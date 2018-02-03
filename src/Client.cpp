#include "Player.hpp"

int main(int argc, char** argv)
{
    const int WIDTH(1000), HEIGHT(1000);

    sf::RenderWindow *window = new sf::RenderWindow(sf::VideoMode(WIDTH, HEIGHT, sf::VideoMode::getDesktopMode().bitsPerPixel), "test", sf::Style::Close, sf::ContextSettings(0, 0, 8, 1, 1, false));
    window->setFramerateLimit(60);

    unsigned short server_port = 2000, remote_port;
    sf::IpAddress server_ip("192.168.1.100"), remote_ip;

    sf::UdpSocket socket;
    sf::Packet send_packet, receive_packet;

    bool first_connection(true);

    int my_client_id, client_id, packet_type;
    float x, y;
    sf::Vector2f pos;

    Player player;

    packet_type = 0;
    send_packet << packet_type;

    socket.setBlocking(false);

    if (socket.send(send_packet, server_ip, server_port) != sf::Socket::Done)
        std::cout << "can't send connection request" << std::endl;
    std::cout << "connection request sent" << std::endl;
    send_packet.clear();

    sf::CircleShape shape;

    while (window->isOpen())
    {
        sf::Event event;

        while (window->pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                packet_type = 2;
                send_packet << packet_type << my_client_id;
                if (socket.send(send_packet, server_ip, server_port) != sf::Socket::Done)
                    std::cout << "can't send disconnection packet" << std::endl;
                send_packet.clear();
                window->close();
                break;

            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Escape)
                {
                    packet_type = 2;
                    send_packet << packet_type << my_client_id;
                    if (socket.send(send_packet, server_ip, server_port) != sf::Socket::Done)
                        std::cout << "can't send disconnection packet" << std::endl;
                    send_packet.clear();
                    window->close();
                }
                break;

            default:
                break;
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            player.move(my_client_id, 0.0f, -10.0f);
            packet_type = 1;
            pos = player.getPosition(my_client_id);
            send_packet << packet_type << my_client_id << pos.x << pos.y;
            if (socket.send(send_packet, server_ip, server_port) != sf::Socket::Done){}
            send_packet.clear();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
        {
            player.move(my_client_id, -10.0f, 0.0f);
            packet_type = 1;
            pos = player.getPosition(my_client_id);
            send_packet << packet_type << my_client_id << pos.x << pos.y;
            if (socket.send(send_packet, server_ip, server_port) != sf::Socket::Done){}
            send_packet.clear();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            player.move(my_client_id, 0.0f, 10.0f);
            packet_type = 1;
            pos = player.getPosition(my_client_id);
            send_packet << packet_type << my_client_id << pos.x << pos.y;
            if (socket.send(send_packet, server_ip, server_port) != sf::Socket::Done){}
            send_packet.clear();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            player.move(my_client_id, 10.0f, 0.0f);
            packet_type = 1;
            pos = player.getPosition(my_client_id);
            send_packet << packet_type << my_client_id << pos.x << pos.y;
            if (socket.send(send_packet, server_ip, server_port) != sf::Socket::Done){}
            send_packet.clear();
        }

        if (socket.receive(receive_packet, remote_ip, remote_port) != sf::Socket::Done && remote_ip != server_ip && remote_port != server_port){std::cout << "problem" << std::endl;}
            //std::cout << "can't receive packet" << std::endl;
        receive_packet >> packet_type;
        
        switch (packet_type)
        {
        case 0:
            if (first_connection)
            {
                first_connection = false;
                if (receive_packet >> my_client_id)
                {
                    receive_packet.clear();
                    player.getPlayerList()->clear();
                    player.addPlayer(my_client_id);
                    std::cout << my_client_id << std::endl;
                }
            }
            else
            {
                receive_packet >> client_id;
                receive_packet.clear();
                if (player.getPlayerList()->find(client_id) == player.getPlayerList()->end())
                    player.addPlayer(client_id);
            }
            break;
            
        case 1:
            receive_packet >> client_id >> x >> y;
            receive_packet.clear();
            //player.setPosition(client_id, x, y);
            std ::cout << client_id << " => " << x << " => " << y << std::endl;
            break;
            
        case 2:
            receive_packet >> client_id;
            receive_packet.clear();
            player.getPlayerList()->erase(player.getPlayerList()->find(client_id));
            break;
            
        default:
            break;
        }

        /*packet_type = 1;
        pos = player.getPosition(my_client_id);
        send_packet << packet_type << my_client_id << pos.x << pos.y;
        if (socket.send(send_packet, server_ip, server_port) != sf::Socket::Done){}*/
            //std::cout << "can't send type 1 packet" << std::endl;
        //std::cout << "type 1 packet sent" << std::endl;

        window->clear(sf::Color::White);

        for (std::map<int, sf::CircleShape>::iterator iter(player.getPlayerList()->begin()); iter != player.getPlayerList()->end(); ++iter)
        {
            window->draw(iter->second);
            //std::cout << "map: " << iter->first << std::endl;
        }    
        
        window->display();
    }

    return 0;
}