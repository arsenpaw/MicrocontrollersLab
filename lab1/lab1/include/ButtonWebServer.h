#include <WebServer.h>
class ButtonWebServer {
    public:
        ButtonWebServer(const char* wifiSSID, const char* wifiPassword);
        void init();
        void handleClient();
    private:
        void handleRoot();
        void handleButtonStatus();
        void handleNotFound();
        WebServer server;
        const char* wifiSSID;
        const char* wifiPassword;
        IPAddress apIP;
    };