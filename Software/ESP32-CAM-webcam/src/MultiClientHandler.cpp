#include "MultiClientHandler.h"

MultiClientHandler::MultiClientHandler() :
  PsychicHandler(),
  _onOpen(NULL),
  _onClose(NULL)
{}

MultiClientHandler::~MultiClientHandler() {
}

MultiClientHandlerClient * MultiClientHandler::getClient(int socket)
{
  PsychicClient *client = PsychicHandler::getClient(socket);

  if (client == NULL)
    return NULL;

  return (MultiClientHandlerClient *)client->_friend;
}

MultiClientHandlerClient * MultiClientHandler::getClient(PsychicClient *client) {
  return getClient(client->socket());
}

esp_err_t MultiClientHandler::handleRequest(PsychicRequest *request)
{
  Serial.println("MultiClientHandler::handleRequest");
  //start our open ended HTTP response
  MultiClientHandlerResponse response(request);
  response.setCode(200);
  response.addHeader("Content-Type", "multipart/x-mixed-replace; boundary=frame");
  response.addHeader("Cache-Control", "no-cache");
  response.addHeader("Connection", "keep-alive");
  
  esp_err_t err = response.send();

  //lookup our client
  PsychicClient *client = checkForNewClient(request->client());
  if (client->isNew)
  {
    /// TODO not likely we need the below code

    //did we get our last id?
    if(request->hasHeader("Last-Event-ID"))
    {
      MultiClientHandlerClient *buddy = getClient(client);
      buddy->_lastId = atoi(request->header("Last-Event-ID").c_str());
    }

    //let our handler know.
    openCallback(client);
  }

  return err;
}

MultiClientHandler * MultiClientHandler::onOpen(MultiClientHandlerCallback fn) {
  _onOpen = fn;
  return this;
}

MultiClientHandler * MultiClientHandler::onClose(MultiClientHandlerCallback fn) {
  _onClose = fn;
  return this;
}

void MultiClientHandler::addClient(PsychicClient *client) {
  client->_friend = new MultiClientHandlerClient(client);
  PsychicHandler::addClient(client);
}

void MultiClientHandler::removeClient(PsychicClient *client) {
  PsychicHandler::removeClient(client);
  delete (MultiClientHandlerClient*)client->_friend;
  client->_friend = NULL;
}

void MultiClientHandler::openCallback(PsychicClient *client) {
  MultiClientHandlerClient *buddy = getClient(client);
  if (buddy == NULL)
  {
    TRACE();
    return;
  }

  if (_onOpen != NULL)
    _onOpen(buddy);
}

void MultiClientHandler::closeCallback(PsychicClient *client) {
  MultiClientHandlerClient *buddy = getClient(client);
  if (buddy == NULL)
  {
    TRACE();
    return;
  }

  if (_onClose != NULL)
    _onClose(getClient(buddy));
}

void MultiClientHandler::send(const char *message, const char *event, uint32_t id, uint32_t reconnect)
{
    // TODO
  String ev;// = generateEventMessage(message, event, id, reconnect);
  for(PsychicClient *c : _clients) {
    ((MultiClientHandlerClient*)c->_friend)->sendEvent(ev.c_str());
  }
}

/*****************************************/
// MultiClientHandlerClient
/*****************************************/

MultiClientHandlerClient::MultiClientHandlerClient(PsychicClient *client) :
  PsychicClient(client->server(), client->socket()),
  _lastId(0)
{
}

MultiClientHandlerClient::~MultiClientHandlerClient(){
}

void MultiClientHandlerClient::send(const char *message, const char *event, uint32_t id, uint32_t reconnect){
    // TODO
    Serial.println("MultiClientHandlerClient::send");
  String ev;// = generateEventMessage(message, event, id, reconnect);
  sendEvent(ev.c_str());
}

void MultiClientHandlerClient::sendEvent(const char *event) {
  int result;
  do {
    result = httpd_socket_send(this->server(), this->socket(), event, strlen(event), 0);
  } while (result == HTTPD_SOCK_ERR_TIMEOUT);

  //if (result < 0)
  //error log here
}

/*****************************************/
// MultiClientHandlerResponse
/*****************************************/

MultiClientHandlerResponse::MultiClientHandlerResponse(PsychicRequest *request) 
  : PsychicResponse(request)
{
}

esp_err_t MultiClientHandlerResponse::send() {

  Serial.println("MultiClientHandlerResponse::send");
  //build our main header
  String out = String();
  out.concat("--frame\r\n");
  out.concat("Content-Type: image/jpeg\r\n");
  out.concat("Cache-Control: no-cache\r\n");
  out.concat("Connection: keep-alive\r\n");
  
  //get our global headers out of the way first
  for (HTTPHeader header : DefaultHeaders::Instance().getHeaders())
    out.concat(String(header.field) + ": " + String(header.value) + "\r\n");

  //separator
  out.concat("\r\n");

  int result;
  do {
    result = httpd_send(_request->request(), out.c_str(), out.length());
  } while (result == HTTPD_SOCK_ERR_TIMEOUT);

  if (result < 0)
    ESP_LOGE(PH_TAG, "EventSource send failed with %s", esp_err_to_name(result));

  if (result > 0)
    return ESP_OK;
  else
    return ESP_ERR_HTTPD_RESP_SEND;
}
