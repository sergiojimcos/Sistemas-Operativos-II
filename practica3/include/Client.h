/*********************************************
*   Project: Práctica 3 de Sistemas Operativos II 
*
*   Program name: Client.h
*
*   Author: Sergio Jiménez
*
*   Date created: 07-04-2020
*
*   Porpuse: Librería en la que se define los diferentes métodos de la clase Client
*
*   Revision History: Reflejado en el repositorio de GitHub
|*********************************************/

#include <fstream>
#include <stdlib.h>
#include "colours.h"
#include <vector>


#ifndef CLIENT_H 
#define CLIENT_H
class Client{

private:
    int id;
    int num_tickets;
    std::vector<int> seats;
    bool pay_tickets;
    bool pay_drink_food;

public:

    Client(int id); // Constructor de Clase
    int get_ID();
    int get_NumTickets();
    bool getPayTicket();
    bool getPayDrinkFood();
    void set_ID(int id);
    void set_NumTickets(int tickets);
    void add_SeatNum(int ticket);
    void setPayTicket(bool pay);
    void setPayDrinkFood(bool pay);
};
#endif