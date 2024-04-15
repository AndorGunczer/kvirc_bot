/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agunczer <agunczer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 16:00:36 by agunczer          #+#    #+#             */
/*   Updated: 2024/04/15 16:28:43 by agunczer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "bot.hpp"

void send_message(int sockfd, const char* message) {
    std::string msg = std::string(message) + "\r\n";
    send(sockfd, msg.c_str(), msg.length(), 0);
}
#define CRLF "/r/n"
int main() {
    // Bot bot("127.0.0.1", 3000, "#wedogreat");
    Bot bot;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(bot.getPort());
    serverAddr.sin_addr.s_addr = inet_addr(bot.getIpAddr().c_str());

    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Error connecting to server" << std::endl;
        close(sockfd);
        return 1;
    }

    char BUFFER[1024];
    send_message(sockfd, ("CAP LS\r\n"));
    while (recv(sockfd, BUFFER, sizeof(BUFFER), 0) < 0) {
        sleep(1);
    }
    bzero(BUFFER, 1024);
    send_message(sockfd, ("CAP REQ :multi-prefix\r\n"));
    while (recv(sockfd, BUFFER, sizeof(BUFFER), 0) < 0) {
        sleep(1);
    }
    bzero(BUFFER, 1024);
    send_message(sockfd, ("CAP END\r\n"));
    bzero(BUFFER, 1024);
    // Send NICK and USER commands to identify with the server
    send_message(sockfd, (("PASS :12345\nNICK " + std::string(bot.getNick()) + "\n" + "USER " + std::string(bot.getUser()) + " 0 irc.localhost.net :" + std::string(bot.getRealName())) + "\r\n").c_str());
    bzero(BUFFER, 1024);
    while (recv(sockfd, BUFFER, sizeof(BUFFER), 0) < 0) {
        sleep(1);
    }
    bzero(BUFFER, 1024);
    // send_message(sockfd, (("NICK " + std::string(NICK)) + "\r\n").c_str());
    // send_message(sockfd, (("USER " + std::string(USER) + " 0 * :" + std::string(REALNAME)) + "\r\n").c_str());

    // Join the channel
    bzero(BUFFER, 1024);
    send_message(sockfd, (("JOIN " + std::string(bot.getChannel()) + " 12345\r\n").c_str()));
    while (recv(sockfd, BUFFER, sizeof(BUFFER), 0) < 0) {
        sleep(1);
    }
    std::cout << "Nachricht vom Server:" << BUFFER << std::endl;
    bzero(BUFFER, 1024);
    // Send a message to the channel
    send_message(sockfd, (("PRIVMSG " + std::string(bot.getChannel()) + " :Hello, I'm a bot!") + "\r\n").c_str());
    // Receive and process messages (loop)
    char buffer[1024];
    while (true) {
        int bytesReceived = recv(sockfd, buffer, 1024, 0);
        if (bytesReceived < 0) {
            std::cerr << "Error receiving data" << std::endl;
            close(sockfd);
            return 1;
        } else if (bytesReceived == 0) {
            std::cout << "Disconnected from server" << std::endl;
            close(sockfd);
            return 0;
        } else {
            // Process received data (parse IRC messages, handle commands, etc.)
            buffer[bytesReceived] = '\0'; // Null-terminate the received data
            std::cout << "Received: " << buffer << std::endl;
            // Add your message parsing and handling logic here
        }
    }

    close(sockfd);
    return 0;
}
