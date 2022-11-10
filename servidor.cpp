#include <winsock2.h>
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <windns.h> //(DNS)
#include <math.h>
#include <ctype.h> //isDigit
#include <fstream> //archivos
#include <ctime>   //tiempo-fecha
#include <string>


using namespace std;
bool cliente_desconectado = false;
bool se_debe_retornar_tamanio = true;



string currentDateTime()
{
    time_t t = time(nullptr);
    tm *now = localtime(&t);

    char buffer[128];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %X", now);
    return buffer;
}

void escribir(string mensaje)
{

    ofstream archivo;

    archivo.open("server.log.txt", ios::app);

    if (archivo.fail())
    {
        cout << "No se pudo abrir el archivo";
        exit(1);
    }
    archivo << currentDateTime() << mensaje << endl; // fecha y hora actual

    archivo.close();
}

int getRenglones()
{

    string texto = "";
    int cant_lineas = 0;
    ifstream archivo;
    archivo.open("server.log.txt", ios::in);
    if (archivo.is_open())
    {
        while (!archivo.eof())
        {
            getline(archivo, texto);
            cant_lineas++;
        }
        archivo.close();
    }
    se_debe_retornar_tamanio = false;
    return cant_lineas;
}
class Server
{
public:
    WSADATA WSAData;
    SOCKET server, client;
    SOCKADDR_IN serverAddr, clientAddr;
    char buffer[1000] = "";
    Server()
    {
        WSAStartup(MAKEWORD(2, 0), &WSAData);
        server = socket(AF_INET, SOCK_STREAM, 0);

        string puerto = "5000";
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(stol("5000"));

        int iVal = 120000;	// milisegundos
        unsigned int sz = sizeof(iVal);
	    int ret = setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, (char *)&iVal, sz);

        bind(server, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
        listen(server,0);
        escribir(" =============================.");
        escribir(" =======Inicia Servidor=======.");
        escribir(" =============================.");
        cout << "El Server se encuentra iniciado en el puerto " + puerto << endl;
        cout << "Escuchando para conexiones entrantes." << endl;
        escribir(" Socket creado, Puerto de escucha:" + puerto + ".");
    }

    void Enviar(string respuesta)
    {
        for (int i = 0; i < (int)respuesta.length(); i++)
        {
            this->buffer[i] = respuesta[i];
        }
        send(client, buffer, sizeof(buffer), 0);
        memset(buffer, 0, sizeof(buffer));
    }

    string Recibir()
    {   
        int recv_size;
        recv_size = recv(client, buffer, sizeof(buffer), 0);
        if (recv_size == SOCKET_ERROR)
        {
            if (WSAGetLastError() == WSAETIMEDOUT)
            {
                cliente_desconectado = true;
                escribir(" Conexion Cerrada por inactividad");
                closesocket(client);
            }
        }
        
        string mensaje = buffer; //guardo el mensaje que recibo

        memset(buffer, 0, sizeof(buffer)); // reinicia el buffer
        return mensaje;
    }

    void ConectarSocket()
    {
        int clientAddrSize = sizeof(clientAddr);
        if ((client = accept(server, (SOCKADDR *)&clientAddr, &clientAddrSize)) != INVALID_SOCKET)
        {
            cout << "Cliente conectado!" << endl;
            escribir(" Conexion Aceptada.");
        }
    }

    void CerrarSocket()
    {
        closesocket(client);
        WSACleanup();
    }

    int getPuerto()
    {
        return (int)ntohs(serverAddr.sin_port);
    }
};

void leerYEnviarTexto(Server *&Servidor)
{

    string texto = "";
    ifstream archivo;
    archivo.open("server.log.txt", ios::in);
    if (archivo.is_open())
    {
        while (!archivo.eof())
        {
            getline(archivo, texto);
            Servidor->Enviar(texto);
            Servidor->Recibir();
        }
        archivo.close();
    }
}

void manejoDePeticion(Server *&Servidor)
{

    char aux[20];
    int num1, num2, resultado = 0, cont = 0, factorial = 1, cantOperadores = 0;
    char *p = NULL;
    string respuesta_error = "";
    char peticionChar[100] = "";
    bool ocurrio_error=false;
    char primerOperador;
    string peticion = "";
    char caracterErroneo;
    peticion = Servidor->Recibir(); // recibe mensaje

    if (peticion[0] == '1')
    {

        for (int y = 1; y < (int)peticion.length(); y++)
        {
            peticionChar[y - 1] = peticion[y];
        }

        for (int y = 0; y < peticion.length() - 1; y++) // le resto el 1 que tiene adelante
        {
            if (peticionChar[y] == '+' || peticionChar[y] == '-' || peticionChar[y] == '/' || peticionChar[y] == '*' || peticionChar[y] == '!' || peticionChar[y] == '^' || isdigit(peticionChar[y]) == true) // Validación de caracteres de la operación
            {
                cont++;
            }
            else
            {
                caracterErroneo = peticionChar[y]; // guardo caracter erroneo
                break;
            }
        }

        if (peticion.size() > 21 || peticion.size() < 2)
        { // maximo caracteres contando el 1
            Servidor->Enviar("La operacion debe tener entre 1 y 20 caracteres");
        }
        else if (cont != peticion.length() - 1) // contador igual a la cantidad de caracteres
        {
            respuesta_error = "No se pudo realizar la operacion, se encontro un caracter no contemplado: [";
            Servidor->Enviar(respuesta_error + caracterErroneo + "]");
        }
        else if (isdigit(peticionChar[0]) == false)
        { // si falta el primer operando
            respuesta_error = "No se pudo realizar la operacion, la operacion esta mal formada:[";
            Servidor->Enviar(respuesta_error + peticionChar[0] + peticionChar[1] + "]");
        }
        else
        {
            for (int y = 0; y < peticion.length() - 1; y++) // le resto el 1 que tiene adelante
            {
                cont++;
                if (peticionChar[y] == '+')
                {
                    cantOperadores++;
                    if(cantOperadores==1){primerOperador=peticionChar[y];}
                    strncpy(aux, peticionChar, cont);
                    num1 = atoi(aux);
                    p = strchr(peticionChar, '+');
                    if (*(p + 1) == '\0') // caracter vacio despues del operador, fin de la cadena
                    {
                        respuesta_error = "No se pudo realizar la operacion, la operacion esta mal formada: [";
                        Servidor->Enviar(respuesta_error + aux + "]");
                        ocurrio_error=true;
                        break;
                    }
                    else if (isdigit(*(p + 1)) == true)
                    {
                        num2 = atoi(p + 1);
                        resultado = num1 + num2;
                    }
                }
                if (peticionChar[y] == '-')
                {
                    cantOperadores++;
                    if(cantOperadores==1){primerOperador=peticionChar[y];}	
                    strncpy(aux, peticionChar, cont);
                    num1 = atoi(aux);
                    p = strchr(peticionChar, '-');
                    if (*(p + 1) == '\0')
                    {
                        respuesta_error = "No se pudo realizar la operacion, la operacion esta mal formada: [";
                        Servidor->Enviar(respuesta_error + aux + "]");
                        ocurrio_error=true;
                        break;
                    }
                    else if (isdigit(*(p + 1)) == true)
                    {
                        num2 = atoi(p + 1);
                        resultado = num1 - num2;
                    }
                }
                if (peticionChar[y] == '/')
                {
                    cantOperadores++;
                    if(cantOperadores==1){primerOperador=peticionChar[y];}
                    strncpy(aux, peticionChar, cont);
                    num1 = atoi(aux);
                    p = strchr(peticionChar, '/');
                    if (*(p + 1) == '\0')
                    {
                        respuesta_error = "No se pudo realizar la operacion, la operacion esta mal formada: [";
                        Servidor->Enviar(respuesta_error + aux + "]");
                        ocurrio_error=true;
                        break;
                    }
                    else if (isdigit(*(p + 1)) == true)
                    {
                        num2 = atoi(p + 1);
                        resultado = num1 / num2;
                    }
                }
                if (peticionChar[y] == '*')
                {
                    cantOperadores++;
                    if(cantOperadores==1){primerOperador=peticionChar[y];}
                    strncpy(aux, peticionChar, cont);
                    num1 = atoi(aux);
                    p = strchr(peticionChar, '*');
                    if (*(p + 1) == '\0')
                    {
                        respuesta_error = "No se pudo realizar la operacion, la operacion esta mal formada: [";
                        Servidor->Enviar(respuesta_error + aux + "]");
                        ocurrio_error=true;
                        break;
                    }
                    else if (isdigit(*(p + 1)) == true)
                    {
                        num2 = atoi(p + 1);
                        resultado = num1 * num2;
                    }
                }
                if (peticionChar[y] == '!')
                {
                    cantOperadores++;
                    if(cantOperadores==1){primerOperador=peticionChar[y];}
                    strncpy(aux, peticionChar, cont);
                    num1 = atoi(aux);
                    for (int j = 1; j <= num1; j++)
                    {
                        factorial = factorial * j;
                    }
                    p = strchr(peticionChar, '!');
                    if (isdigit(*(p + 1)) == true)
                    {
                        respuesta_error = "No se pudo realizar la operacion, la operacion esta mal formada: [";
                        Servidor->Enviar(respuesta_error + peticionChar[y - 1] + p + "]");
                        ocurrio_error=true;
                        break;
                    }
                    else
                    {
                        resultado = factorial;
                    }
                }
                if (peticionChar[y] == '^')
                {
                    cantOperadores++;
                    if(cantOperadores==1){primerOperador=peticionChar[y];}
                    strncpy(aux, peticionChar, cont);
                    num1 = atoi(aux);
                    p = strchr(peticionChar, '^');
                    if (*(p + 1) == '\0')
                    {
                        respuesta_error = "No se pudo realizar la operacion, la operacion esta mal formada: [";
                        Servidor->Enviar(respuesta_error + aux + "]");
                        ocurrio_error=true;
                        break;
                    }
                    else if (isdigit(*(p + 1)) == true)
                    {
                        num2 = atoi(p + 1);
                        resultado = pow(num1, num2) + 0.5; // redondear
                    }
                }
            }
            if (p == NULL)
            {
                string aux=" ";
                Servidor->Enviar(aux+peticionChar);
            }
            else if (cantOperadores > 1) // cantidad de operadores , tiene que ser uno
            {   cout<<peticionChar[cont]<<endl;
                respuesta_error = "No se pudo realizar la operacion, la operacion esta mal formada:[";
                Servidor->Enviar(respuesta_error +primerOperador +p + "]");
            }
            else if(ocurrio_error!=true)
            {
                delete p;
                Servidor->Enviar("Resultado: "+to_string(resultado));
            }
        }
    }
    else if (peticion[0] == '2')
    {
        if (se_debe_retornar_tamanio == true)
        {
            Servidor->Enviar(to_string(getRenglones()));
        }
        leerYEnviarTexto(Servidor);
        se_debe_retornar_tamanio=true;
        Servidor->Enviar("");
        
    }
    else if (peticion[0] == '3')
    {
        Servidor->Enviar("");
        cliente_desconectado = true;
    }
}

int main()
{
    Server *Servidor = new Server();
    Servidor->ConectarSocket();
    while (true)
    {

        while (cliente_desconectado == false)
        {
            manejoDePeticion(Servidor);
        }
        cliente_desconectado = false;
        Servidor->ConectarSocket();
    }
}
