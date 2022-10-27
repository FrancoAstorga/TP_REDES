#include <iostream>
#include <winsock2.h>
#include <string>
#include <conio.h>
#include <fstream> //Lib. para trabajar con archivos
#include <ctime>   //Lib. para trabajar con fechas / tiempos
#include <cstdlib>
#include <stdio.h>
#include <windns.h>
#include <math.h>
#include <ctype.h> //ISdigit

using namespace std;

string usuarioConectado;

bool serverIniciado;
string ip_global;
string puerto_global;

string currentDateTime() {
    time_t t = time(nullptr);
    tm* now = localtime(&t);

    char buffer[128];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %X", now);
    return buffer;
}

void escribir(string mensaje){

    ofstream archivo;

    archivo.open("server.log.txt",ios::app | ios::binary);//nombre y modo , out si no existe se crea

    if(archivo.fail()){
        cout<<"No se pudo abrir el archivo";
        exit(1);
    }
    archivo<<currentDateTime()<<mensaje<<endl;//fecha y hora actual

    archivo.close();

}


string leerArchivo()
{

    string texto = "";
    string aux="";
    ifstream archivo;
    archivo.open("server.log.txt",ios::in | ios::binary);
    if(archivo.is_open()){
        while(!archivo.eof()){
            getline(archivo,texto);
            aux.append(texto);
            }
            archivo.close();
        }
    return aux;
}


class Server
{
public:
    WSADATA WSAData;
    SOCKET server, client;
    SOCKADDR_IN serverAddr, clientAddr;
    char buffer[2000];//para poder guardar el txt
    Server()
    {
        WSAStartup(MAKEWORD(2, 0), &WSAData);
        server = socket(AF_INET, SOCK_STREAM, 0);

        while (!serverIniciado)
        {
            string ipInput = "127.0.0.1";
            string puertoInput = "5000";

            cout << "Configurando server..." << endl;
            cout << endl;

            // while(ipInput==""){
            //     cout<<"Ingrese la direccion IP: ";
            //     cin>>ipInput;
            //     system("CLS");
            // }
            //    while(puertoInput==""){
            //     cout<<"Ingrese el puerto: ";
            //     cin>>puertoInput;
            //     system("CLS");
            // }

            serverIniciado = true;
            ip_global = ipInput;
            puerto_global = puertoInput;
        }

        IniciarServer();
        //**TIMER
        // int opt=1;
        // setsockopt(server, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
        // //Hay dos tipos de opciones de socket: opciones booleanas que habilitan o deshabilitan una característica
        // // o comportamiento y opciones que requieren una estructura o un valor entero. Para habilitar una opción booleana, 
        // //el parámetro optval apunta a un entero distinto de cero. Para deshabilitar la opción optval apunta a un número entero igual a cero. 
        // //El parámetro optlen debe ser igual a sizeof(int)para las opciones booleanas. Para otras opciones, optval apunta a un entero o estructura 
        // //que contiene el valor deseado para la opción, y optlen es la longitud del entero o estructura.
        // int Timer = 6000;//6 segundos
        // //iVal = 1000;
        // //SO_RCVTIMEO-->Establece el tiempo de espera, en milisegundos, para bloquear las llamadas recibidas.
        // if(setsockopt(server, SOL_SOCKET, SO_RCVTIMEO,(char *) &Timer, sizeof(Timer)) == SOCKET_ERROR )
        // {
        //     cout<<"TIEMPO AGOTADO  "<<endl;

        //     closesocket(server);
        //     WSACleanup();
        // }


        bind(server, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
        listen(server, 0);
        cout << "El Server se encuentra iniciado en el puerto " + puerto_global << endl;
        cout << "Escuchando para conexiones entrantes." << endl;
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
        fd_set fds;
        struct timeval tv;

        tv.tv_sec = 10;//2 minutos
        tv.tv_usec = 0;

        FD_ZERO(&fds);
        FD_SET(client, &fds);

        int n = select(client, &fds, NULL, NULL, &tv);

        if (n == 0)
        {
            //cout << "Cliente desconectado , tiempo maximo de sesion agotado" << endl;
            //CerrarSocket(usuarioConectado);
            closesocket(client);
        }
        recv(client, buffer, sizeof(buffer), 0); // recibe mensaje

        string buff = buffer; // guardo valor recibido

        memset(buffer, 0, sizeof(buffer)); // vacia el buffer
        return buff;
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

    void CerrarSocket(string username)
    {
        closesocket(client);
        WSACleanup();
        cout << "Socket cerrado, cliente desconectado." << endl;
        escribir(" Conexion Cerrada por Inactividad.");
    }

    void IniciarServer()
    {
        int n = ip_global.length();
        char ip[n + 1];
        strcpy(ip, ip_global.c_str());
        serverAddr.sin_addr.s_addr = inet_addr(ip);
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(stol(puerto_global));
    }

    int getPuerto()
    {
        return (int)ntohs(serverAddr.sin_port);
    }
};

void manejarPeticion(Server *&Servidor)
{

    char aux[20];
    int num1, num2, resultado, cont = 0, i = 0, factorial = 1, contador = 0,cantOperadores=0;
    char *p = NULL;
    string operacion = " ";
    char peticionChar[20] = "";
    string peticion = "";
    char caracterErroneo;
    bool bandera=false;
    peticion = Servidor->Recibir();//en caso de recibir un espacio vacio corta ahi el mensaje
    cout<<"Peticion: "<<peticion<<endl;
    cout<<"Tamanio: "<<peticion.size()<<endl;
    
    if(peticion[0] == '1')
    {
        
        for (int y = 1; y < (int)peticion.length(); y++)
        {
            peticionChar[y - 1] = peticion[y];
        }

        for (int y = 0; y < peticion.length() - 1; y++) // le resto el 1 que tiene adelante
        {
            if (peticionChar[y] == '+' || peticionChar[y] == '-' || peticionChar[y] == '/' || peticionChar[y] == '*' || peticionChar[y] == '!' || peticionChar[y] == '^' || isdigit(peticionChar[y]) != XST_NULL || peticionChar[y] == ' ' ) // Validación de caracteres de la operación
            {
                cont++;
            }
            else
            {
                caracterErroneo = peticionChar[y]; // guardo caracter erroneo
                break;
            }
        }
        cout<<"226--> "<<peticionChar<<endl;
        //falta si ingresa cadena vacia
        if(peticion.size()>21){//maximo caracteres contando el 1
            cout<<"La operacion debe tener entre 1 y 20 caracteres"<<endl;
        }
        else if(cont != peticion.length() - 1) // contador igual a la cantidad de caracteres
        {
            cout << "No se pudo realizar la operacion, se encontro un caracter no contemplado: [" << caracterErroneo << "]" << endl;
        }
        else if(isdigit(peticionChar[0])==XST_NULL){//si falta el primer operando
            cout<<"No se pudo realizar la operacion,falta primer operando: [a]"<<endl;
        }
        else
        {
            
            for (int y = 0; y < peticion.length() - 1; y++) // le resto el 1 que tiene adelante
            {
                // operacion=operacion+peticion[y];
                cout<<"183---> "<<peticionChar[y]<<endl;
                cont++;
                if(peticionChar[y]==' '){
                    bandera=true;
                    cout<<"No se pudo realizar la operacion, la operacion esta mal formada: [a]"<<endl;
                    break;
                }
                if (peticionChar[y] == '+')
                {
                    cantOperadores++;
                    strncpy(aux, peticionChar, cont);
                    num1 = atoi(aux);
                    p = strchr(peticionChar, '+');
                    if (*(p+1)=='\0'){//caracter vacio despues del operador, fin de la cadena
                        cout<<"No se pudo realizar la operacion, la operacion esta mal formada: [c]"<<endl;
                        break;
                    }
                    num2 = atoi(p + 1);
                    resultado = num1 + num2;
                }
                if (peticionChar[y] == '-')
                {
                    cantOperadores++;
                    strncpy(aux, peticionChar, cont);
                    num1 = atoi(aux);
                    p = strchr(peticionChar, '-');
                     if (*(p+1)=='\0'){//caracter vacio despues del operador,fin de la cadena
                        cout<<"No se pudo realizar la operacion, la operacion esta mal formada: [c]"<<endl;
                        break;
                    }
                    num2 = atoi(p + 1);
                    resultado = num1 - num2;
                }
                if (peticionChar[y] == '/')
                {
                    cantOperadores++;
                    strncpy(aux, peticionChar, cont);
                    num1 = atoi(aux);
                    p = strchr(peticionChar, '/');
                     if (*(p+1)=='\0'){//caracter vacio despues del operador,fin de la cadena
                        cout<<"No se pudo realizar la operacion, la operacion esta mal formada: [c]"<<endl;
                        break;
                    }
                    num2 = atoi(p + 1);
                    resultado = num1 / num2;
                }
                if (peticionChar[y] == '*')
                {
                    cantOperadores++;
                    strncpy(aux, peticionChar, cont);
                    num1 = atoi(aux);
                    p = strchr(peticionChar, '*');
                     if (*(p+1)=='\0'){//caracter vacio despues del operador,fin de la cadena
                        cout<<"No se pudo realizar la operacion, la operacion esta mal formada: [c]"<<endl;
                        break;
                    }
                    num2 = atoi(p + 1);
                    resultado = num1 * num2;
                }
                if (peticionChar[y] == '!')
                {
                    cantOperadores++;
                    strncpy(aux, peticionChar, cont);
                    num1 = atoi(aux);
                    for (int j = 1; j <= num1; j++)
                    {
                        factorial = factorial * j;
                    }
                    p = strchr(peticionChar, '!');
                     if (*(p+1)!='\0'){//caracter vacio despues del operador,fin de la cadena
                        cout<<"No se pudo realizar la operacion, la operacion esta mal formada: [a!b]"<<endl;
                        break;
                    }
                    resultado = factorial;
                }
                if (peticionChar[y] == '^')
                {
                    cantOperadores++;
                    strncpy(aux, peticionChar, cont);
                    num1 = atoi(aux);
                    p = strchr(peticionChar, '^');
                     if (*(p+1)=='\0'){//caracter vacio despues del operador,fin de la cadena
                        cout<<"No se pudo realizar la operacion, la operacion esta mal formada: [c]"<<endl;
                        break;
                    }
                    num2 = atoi(p + 1);
                    resultado = pow(num1, num2) + 0.5; // redondear
                }
            }
            //bandera para saber si falta primer operando
            if(p==NULL && bandera==false){//Validación de operación, espacio entre el principio y operador o falta operador
                cout<<"No se pudo realizar la operacion, la operacion esta mal formada:[b]"<<endl;
            }
            if(cantOperadores>1){//cantidad de operadores , tiene que ser uno
                cout<<"No se pudo realizar la operacion, la operacion esta mal formada:[bb]"<<endl;
            }
            delete p;
            Servidor->Enviar(to_string(resultado));
        }

    
    }
    //*************************************************************************************************
    else if(peticion[0] == '2'){
        Servidor->Enviar(leerArchivo());
    }



}






int main()
{

    while (true)
    {
        

        Server *Servidor = new Server();

        string puertoEscucha = "Socket creado, Puerto de escucha: " + to_string(Servidor->getPuerto());
        escribir(" =============================.");
        escribir(" =======Inicia Servidor=======.");
        escribir(" =============================.");
        escribir(" Socket creado, Puerto de escucha:"+puerto_global+".");

        Servidor->ConectarSocket();
        manejarPeticion(Servidor);
        Servidor->CerrarSocket("franco");

        // system("cls");
    }

    main();
}
