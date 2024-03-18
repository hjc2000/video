using WsHttpAgentClient;

Uri listen_uri = new("ws://localhost:8848/ws");
_ = new WsHttpClient("001449a01372", listen_uri).RunAsync();
await new WsHttpClient("001449a01373", listen_uri).RunAsync();
