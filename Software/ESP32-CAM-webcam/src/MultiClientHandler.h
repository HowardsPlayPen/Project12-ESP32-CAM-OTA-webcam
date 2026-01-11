
#ifndef MultiClientHandler_H_
#define MultiClientHandler_H_

#include "PsychicCore.h"
#include "PsychicHandler.h"
#include "PsychicClient.h"
#include "PsychicResponse.h"

class MultiClientHandler;
class MultiClientHandlerResponse;
class MultiClientHandlerClient;
class PsychicResponse;

typedef std::function<void(MultiClientHandlerClient *client)> MultiClientHandlerCallback;


class MultiClientHandlerClient : public PsychicClient {
  friend MultiClientHandler;

  protected:
    uint32_t _lastId;

  public:
    MultiClientHandlerClient(PsychicClient *client);
    ~MultiClientHandlerClient();

    uint32_t lastId() const { return _lastId; }
    void send(const char *message, const char *event=NULL, uint32_t id=0, uint32_t reconnect=0);
    void sendEvent(const char *event);
};

class MultiClientHandler : public PsychicHandler {
  private:
    MultiClientHandlerCallback _onOpen;
    MultiClientHandlerCallback _onClose;

  public:
    MultiClientHandler();
    ~MultiClientHandler();

    MultiClientHandlerClient * getClient(int socket) override;
    MultiClientHandlerClient * getClient(PsychicClient *client) override;
    void addClient(PsychicClient *client) override;
    void removeClient(PsychicClient *client) override;
    void openCallback(PsychicClient *client) override;
    void closeCallback(PsychicClient *client) override;

    MultiClientHandler *onOpen(MultiClientHandlerCallback fn);
    MultiClientHandler *onClose(MultiClientHandlerCallback fn);

    esp_err_t handleRequest(PsychicRequest *request) override final;

    void send(const char *message, const char *event=NULL, uint32_t id=0, uint32_t reconnect=0);
};

class MultiClientHandlerResponse: public PsychicResponse {
  public:
    MultiClientHandlerResponse(PsychicRequest *request);
    virtual esp_err_t send() override;
};

#endif