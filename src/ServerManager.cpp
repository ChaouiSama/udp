#include "ServerManager.hpp"

ServerManager::ServerManager(unsigned short port)
{
    m_socket.setBlocking(false);
    
    if (m_socket.bind(port) != sf::Socket::Done)
        std::cout << "unable to bind port: " << port << std::endl;
    std::cout << "server is listening on port: " << port << std::endl;
    NEXT_AVAILABLE_ID = 0;
}

void ServerManager::handleReceive()
{
    if (m_socket.receive(m_receive_packet, m_client_ip, m_client_port) == sf::Socket::Done)
    {
        m_receive_packet >> m_packet_type;

        switch (m_packet_type)
        {
        case 0:
            handleClientConnection(m_client_ip, m_client_port);
            break;

        case 1:
            handleSend(m_receive_packet, m_client_ip, m_client_port);
            break;

        case 2:
            handleClientDisconnection(m_client_ip, m_client_port);
            break;

        default:
            break;
        }
    }
}

void ServerManager::handleClientConnection(sf::IpAddress client_ip, unsigned short client_port)
{
    m_receive_packet.clear();

    NEXT_AVAILABLE_ID++;
    m_packet_type = 0;
    
    m_clients.emplace(NEXT_AVAILABLE_ID, std::pair<sf::IpAddress, unsigned short>(client_ip, client_port));

    for (std::multimap<int, std::pair<sf::IpAddress, unsigned short>>::reverse_iterator iterp(m_clients.rbegin()); iterp != m_clients.rend(); ++iterp)
    {
        for (std::multimap<int, std::pair<sf::IpAddress, unsigned short>>::reverse_iterator iteri(m_clients.rbegin()); iteri != m_clients.rend(); ++iteri)
        {
            m_send_packet << m_packet_type << iteri->first;
            
            if (m_socket.send(m_send_packet, std::get<0>(iterp->second), std::get<1>(iterp->second)) != sf::Socket::Done)
                std::cout << "unable to send first connection id[" << iteri->first << "] to " << std::get<0>(iterp->second) << ":" << std::get<1>(iterp->second) << std::endl;
        
            std::cout << "first connection id[" << iteri->first << "] sent to " << std::get<0>(iterp->second) << ":" << std::get<1>(iterp->second) << std::endl;
        
            m_send_packet.clear();
        }
    }
}

void ServerManager::handleSend(sf::Packet packet, sf::IpAddress client_ip, unsigned short client_port)
{
    m_receive_packet >> m_client_id >> m_x >> m_y;
    m_receive_packet.clear();
    std::cout << m_client_id << " => " << m_x << " => " << m_y << std::endl;

    m_packet_type = 1;
    m_send_packet << m_packet_type << m_client_id << m_x << m_y;

    for (std::multimap<int, std::pair<sf::IpAddress, unsigned short>>::iterator iterp(m_clients.begin()); iterp != m_clients.end(); ++iterp)
    {
        for (std::multimap<int, std::pair<sf::IpAddress, unsigned short>>::iterator iteri(m_clients.begin()); iteri != m_clients.end(); ++iteri)
        {
            if (std::get<0>(iterp->second) != client_ip && std::get<1>(iterp->second) != client_port)
            {
                if (m_socket.send(m_send_packet, std::get<0>(iterp->second), std::get<1>(iterp->second)) != sf::Socket::Done)
                    std::cout << "unable to send type 1 packet" << std::endl;
                std::cout << "type 1 packet sent to id[" << iteri->first << "] => " << std::get<0>(iterp->second) << ":" << std::get<1>(iterp->second) << std::endl;
            }
        }
    }

    m_send_packet.clear();
}

void ServerManager::handleClientDisconnection(sf::IpAddress client_ip, unsigned short client_port)
{
    m_receive_packet >> m_client_id;
    m_receive_packet.clear();

    m_packet_type = 2;
    m_send_packet << m_packet_type << m_client_id;
    
    m_clients.erase(m_clients.find(m_client_id));

    for (std::multimap<int, std::pair<sf::IpAddress, unsigned short>>::iterator iter(m_clients.begin()); iter != m_clients.end(); ++iter)
    {    
        if (std::get<0>(iter->second) != client_ip && std::get<1>(iter->second) != client_port)
        {
            if (m_socket.send(m_send_packet, std::get<0>(iter->second), std::get<1>(iter->second)) != sf::Socket::Done)
                std::cout << "unable to send deconnection packet received from id[" << m_client_id << "] to " << std::get<0>(iter->second) << ":" << std::get<1>(iter->second) << std::endl;
        
            std::cout << "deconnection packet received from id[" << m_client_id << "] sent to " << std::get<0>(iter->second) << ":" << std::get<1>(iter->second) << std::endl;
        }
        else
            std::cout << "deconnection packet received from id[" << m_client_id << "] sent to no one" << std::endl;
    }
        
    m_send_packet.clear();
}