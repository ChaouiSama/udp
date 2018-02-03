#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <iostream>

class ServerManager
{
public:
    ServerManager(unsigned short port);

    void handleReceive();

    void handleClientConnection(sf::IpAddress client_ip, unsigned short client_port);
    
    void handleSend(sf::Packet packet, sf::IpAddress client_ip, unsigned short client_port);

    void handleClientDisconnection(sf::IpAddress client_ip, unsigned short client_port);

private:
    sf::UdpSocket m_socket;

    sf::Packet m_send_packet, m_receive_packet;

    sf::IpAddress m_client_ip;
    unsigned short m_client_port;

    int NEXT_AVAILABLE_ID;
    int m_client_id;
    int m_packet_type;
    float m_x, m_y;

    std::map<int, std::pair<sf::IpAddress, unsigned short>> m_clients;
};

#endif /* SERVERMANAGER_HPP */