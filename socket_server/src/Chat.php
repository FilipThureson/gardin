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
        /* $msg = {
            "type": "type",
            "msg": "msg"
        } */
        $msg = json_decode($msg); // Gör om JSON-string till php object för enklare hantering
        
        switch ($msg->type) { // Switch case stats på type i $msg
            case "client_connect":
            //När en client kopplas upp skickas de till resterande clients och framörallt NodeMCU
              foreach ($this->clients as $client) {
                if ($from !== $client) {
                    print_r($msg);
                    $client->send(json_encode($msg));
                }
              }
              break;
              case "server_connect":
                //När NodeMcu Server kopplas upp till socket-servern så skickas det ut tilla alla clienter så de kan uppdatera sin sida  
                foreach ($this->clients as $client) {
                  if ($from !== $client) {
                      $client->send(json_encode($msg));
                  }
                }
                break;
            case "send_state":
                //NodeMcu skickar sin state på gardinen till alla clienter för att kunna uppdatera utseendet
                foreach ($this->clients as $client) {
                    if ($from !== $client) {
                        $client->send(json_encode($msg));
                    }
                }
                break;
            case "client_action":
                //client skickar frågan om att flytta gardinen
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