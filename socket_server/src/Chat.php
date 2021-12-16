<?php
namespace MyApp;
use Ratchet\MessageComponentInterface;
use Ratchet\ConnectionInterface;

class Chat implements MessageComponentInterface {
    protected $clients;

    public function __construct() {
        $this->clients = new \SplObjectStorage;
    }

    public function onOpen(ConnectionInterface $conn) {
        // Store the new connection to send messages to later
        $this->clients->attach($conn);

        echo "New connection! ({$conn->resourceId})\n";
    }

    public function onMessage(ConnectionInterface $from, $msg) {
        $msg = json_decode($msg);
        $numRecv = count($this->clients) - 1;
        //echo sprintf('Connection %d sending message "%s" to %d other connection%s' . "\n"
        //    , $from->resourceId, $msg, $numRecv, $numRecv == 1 ? '' : 's');

        //foreach ($this->clients as $client) {
        //    if ($from !== $client) {
                // The sender is not the receiver, send to each client connected
        //        $client->send($msg);
        //    }
        //}
        switch ($msg->type) {
            case "client_connect":
              foreach ($this->clients as $client) {
                if ($from !== $client) {
                    print_r($msg);
                    $client->send(json_encode($msg));
                }
              }
              break;
              case "server_connect":
                foreach ($this->clients as $client) {
                  if ($from !== $client) {
                      $client->send(json_encode($msg));
                  }
                }
                break;
            case "send_state":
                foreach ($this->clients as $client) {
                    if ($from !== $client) {
                        $client->send(json_encode($msg));
                    }
                }
                break;
            case "client_action":
                foreach ($this->clients as $client) {
                    if ($from !== $client) {
                        $client->send(json_encode($msg));
                    }
                }
                break;
        }
    }

    public function onClose(ConnectionInterface $conn) {
        // The connection is closed, remove it, as we can no longer send it messages
        $this->clients->detach($conn);

        echo "Connection {$conn->resourceId} has disconnected\n";
    }

    public function onError(ConnectionInterface $conn, \Exception $e) {
        echo "An error has occurred: {$e->getMessage()}\n";

        $conn->close();
    }
}