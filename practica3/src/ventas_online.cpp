/*********************************************
*   Project: Práctica 3 de Sistemas Operativos II 
*
*   Program name: ventas_online.cpp
*
*   Author: Sergio Jiménez
*
*   Date created: 06-04-2020
*
*   Porpuse: Gestión de procesos para realización paralela de hilos
*
*   Revision History: Reflejado en el repositorio de GitHub
|*********************************************/

#include <iostream>
#include <regex>
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>


#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/colours.h"
#include "../include/SemCounter.h"
#include "../include/TicketMsg.h"
#include "../include/Food_DrinkMsg.h"

#define SNACK_STALL 3 /*Definimos el número de puestos de comida*/
#define MAX_FOOD 20 /*Definimos el número máximos de bebidas y de palomitas que va a tener cada punto*/


void client_request(int id_client);
void initialize_environment(int num_clients);
void assign_seats(int num_tickets, int num_thread);
void request_information(int seat, int num_thread);
void pay_service();
int generate_ticket_random();
void ticket_sale();
void snack_stall (int id_snack_stall);
void stock_manager();

/*Semáforo para el hilo reponedor*/

/*Semáforos para el sistema de pago */
std::mutex pay_system;
std::mutex pay_system_wait;
std::mutex mutex_payment;


std::mutex s_ticket_sale;
std::mutex s_ticket_request;
std::mutex s_wait_tickets;
std::mutex s_manager;
std::mutex s_request_tickets;
std::mutex s_client_food_drink;

/*Variables de condición*/
std::condition_variable cv_shift_ticket;
std::condition_variable cv_shift_snack_stall;

std::vector<std::thread> queue_client_request;/*Cola de espera de entrada al cine de cada uno de los clientes*/
std::vector<std::thread> g_snack_stall; /*Cola donde se guarda los hilos de los puestos de comidas*/
std::vector<std::thread> queue_snack_stall; /*Cola donde se mueve de la cola de tickets a la cola de puestos de comida*/

std::queue<TicketMsg> queue_tickets;
std::queue<Food_DrinkMsg>queue_food_drink;

int shift_ticket = -1;
int shift_snack_stall = -1; 
int cinema[6][12];
int num_seats = 72;
bool with_tickets;

/*Definismos el stock que va a tener cada uno de los puestos de comida 0->Comida y 1->Bebida*/
int stock_stall_1[2];
int stock_stall_2[2];
int stock_stall_3[2]; 

int main(int argc, char *argv[]){

    int num_clients;
    
    if(argc!=2){
        
        std::cerr << FRED("[MANAGER] El número de argumentos es incorrecto <número_clientes>.") <<std::endl;
        exit(EXIT_FAILURE);
    }
    try{
    
        num_clients = std::stoi(argv[1]);
    
    }catch(const std::exception &e){
        std::cerr << FRED("[MANAGER] Ha habido un error en la captación del argumento") <<std::endl;
        exit(EXIT_FAILURE);
    }
   
    initialize_environment(num_clients); /*Inicializamos las butacas en la sala de cine*/

    std::thread th_stock_manager(stock_manager); /*Hilo para el reponedor*/
    
    /*Iniciación de semáforos*/
    s_manager.lock();
    s_ticket_request.lock();
    s_wait_tickets.lock();
    pay_system_wait.lock();
    pay_system.lock();

    std::thread th_payment_service (pay_service);
    
    std::thread th_ticket_sale(ticket_sale);

    for (int i = 1;i<=SNACK_STALL;i++){
        g_snack_stall.push_back(std::thread(snack_stall,i));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    for (int i =1; i<= num_clients;i++){
        
        queue_client_request.push_back(std::thread(client_request,i));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    std::unique_lock<std::mutex> lk(s_ticket_sale);
    
    
    for(int i=1;i<=num_clients;i++){
        
        if(i > 1){
            
            lk.lock();
        }
        std::cout << FRED("[MANAGER] El turno es para el cliente ") << i <<std::endl;
        shift_ticket = i;
        cv_shift_ticket.notify_all();
        lk.unlock();
        s_manager.lock();
        
    }
    

    /*Esperamos a que finalice cada uno de los clientes*/
    for_each(queue_client_request.begin(), queue_client_request.end(),std::mem_fn(&std::thread::join));
    th_ticket_sale.join();
    th_payment_service.join();

    return EXIT_SUCCESS;
}

void initialize_environment(int num_clients){
    /*Inicializamos el entorno del programa*/
    for (int i = 0;i < 6; i++ ){
        for(int j = 0;j < 12; j++){
            cinema[i][j] = 0;
        }
    }

    for (int i = 0; i<2;i++){
        
        stock_stall_1[i] = MAX_FOOD;
        stock_stall_2[i] = MAX_FOOD;
        stock_stall_3[i] = MAX_FOOD;
    }     
}

void client_request(int id_client){
    /*Simulación de las peticiones de los clientes*/
    std::cout << FYEL("[CLIENTE ") << id_client << FYEL("] estoy en la cola...") << std::endl;
    std::unique_lock<std::mutex> lk(s_ticket_sale);
    

    cv_shift_ticket.wait(lk,[id_client]{return (shift_ticket==id_client);});
    std::cout << FGRN("[CLIENTE ") << id_client << FGRN("] es mi turno, voy a por las entradas")<<std::endl;
    lk.unlock();

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    TicketMsg ticket_request(id_client);
    ticket_request.generate_ticket_random();
    queue_tickets.push(ticket_request);

    s_ticket_request.unlock();
    s_wait_tickets.lock();
    
    s_manager.unlock();
    ticket_request.setPayd(with_tickets);

    if(ticket_request.getPayd()){
        Food_DrinkMsg food_drink_request(id_client);

        food_drink_request.drink_food_random();
        queue_food_drink.push(food_drink_request);

    }
}

void pay_service(){
    /*Simulación del sistema del pago*/
    std::cout << FYEL("[SISTEMA DE PAGO] SISTEMA DE PAGO CREADO") <<std::endl;

    while (true){
        pay_system.lock();
        
        std::cout << FYEL("[SISTEMA DE PAGO] Cliente ")<<shift_ticket<< FYEL(" ha pagado las entradas ")<<num_seats<<std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        pay_system_wait.unlock();
    }
}

void ticket_sale(){
    /*Simulación de la taquilla para la venta de los tickets*/
    int remain_tickets;
    
    std::cout << FRED("[MANAGER] TAQUILLA ABIERTA") <<std::endl;
    while (true)
    {
        
        s_ticket_request.lock();
        TicketMsg msg_ticket = queue_tickets.front();
        queue_tickets.pop();
        
        std::cout << "[TAQUILLA]: Venta de " << msg_ticket.get_NumTickets() << " tickets a cliente "<< msg_ticket.get_ID() << "...\n";
        s_request_tickets.lock();

        remain_tickets = num_seats - msg_ticket.get_NumTickets();

        if(remain_tickets<=0){
            
            with_tickets = false;
        }else{
            
            assign_seats(msg_ticket.get_NumTickets(),msg_ticket.get_ID());
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            with_tickets = true;
            
            mutex_payment.lock();
            pay_system.unlock();
            /*Rendevouz con el sistema de pago*/
            pay_system_wait.lock();
            mutex_payment.unlock();
        }

        s_request_tickets.unlock();

        queue_snack_stall.push_back(std::move(queue_client_request.front()));/*Movemos la solicitud de clientes a la nueva cola*/
        queue_client_request.erase(queue_client_request.begin());
            
        s_wait_tickets.unlock();

    }
    
}

void request_information(int seat, int num_thread){
    /*Función para la impresión de la solicitud de los tickets*/
    std::string inf_row;
    std::string inf_column;

    int row = (num_seats-1) /12;
    int column = (num_seats-1)%12;
    
    if(row == 0 || row == 1){ /*Información de las filas*/

        inf_row = " cerca ";
    }else if (row == 2||row == 3){
        
        inf_row = " medio ";
    }else if(row == 4 || row == 5){
        
        inf_row = " lejos ";
    }

    if(column == 0 || column == 1 || column == 2 || column == 3){

        inf_column = " izquierda ";
    }else if(column == 4 || column == 5 || column == 6 || column == 7){

        inf_column = " medio ";
    }else if (column == 8 || column == 9 || column == 10 || column == 11){

        inf_column = " derecha ";
    }

    std::cout << FBLU("SOLICITUD ENTRADAS < ")<<num_thread+1<< FBLU(" > < ")<<seat<< FBLU(" > <")<<inf_row<<FBLU("> <") << inf_column<< FBLU(">")<<std::endl;

}

void assign_seats(int num_tickets, int num_thread){
    /*Función para asignar asientos a los clientes*/
    for (int i = 0; i < num_tickets; i++)
    {   
        request_information(num_tickets,num_thread);
        num_seats--;

    }
}

void snack_stall (int id_snack_stall){
/*Simulación de los puestos para la venta de la comida*/

    std::cout << FMAG("[PUESTO DE COMIDA] ") << id_snack_stall<< FMAG(" abierto")<<std::endl;
    std::unique_lock lock_stall (s_client_food_drink);


}

void stock_manager(){
    /*Función que va a realizar el stock manager*/
    std::cout << FCYN("[REPONEDOR] disponible")<<std::endl;

}

