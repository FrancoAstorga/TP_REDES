#include <winsock2.h>
#include <iostream>
#include <string>
#include <cstdlib>


using namespace std;
bool tiempo_espera_se_agoto = false;

string IngresarPuerto()
{
    system("cls");
    string PUERTO = "";
    
    cout<<"Ingrese el puerto: ";
    cin>>PUERTO;


    return PUERTO;
}

class Client
{
public:
    WSADATA WSAData;
    SOCKET server;
    SOCKADDR_IN addr;
    char buffer[1000] = "";
    Client()
    {
        cout << "Conectando al servidor..." <<endl<< endl;
        WSAStartup(MAKEWORD(2, 0), &WSAData);
        server = socket(AF_INET, SOCK_STREAM, 0);
        string validacion_puerto = IngresarPuerto();
        if(validacion_puerto!="5000"){CerrarSocket();}
        
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        addr.sin_family = AF_INET;
        addr.sin_port = htons(stol(validacion_puerto));

        
        if (connect(server, (SOCKADDR *)&addr, sizeof(addr)) == 0)
        {
            cout << "Conectado al Servidor!" << endl;
        }
        else
        {
            cout << "Error al conectarse. Vuelva a intentarlo mas tarde." << endl;
            CerrarSocket();
            system("pause");
            exit(1);
        }
    }
    void Enviar(string mensaje)
    {

        for (int i = 0; i < mensaje.length(); i++)
        {
            this->buffer[i] = mensaje[i];
        }

        send(server, buffer, sizeof(buffer), 0);//cliente envia mensaje
        memset(buffer, 0, sizeof(buffer));
    }

    string Recibir()
    {
        int iResult = 0;
        iResult = recv(server, buffer, sizeof(buffer), 0);//recibe mensaje del servidor
        string respuesta_cliente = buffer;

        // if (iResult == -1)
        // {
        //     cout << "Cliente desconectado por inactividad" << endl;
        //     tiempo_espera_se_agoto = true;
        //     system("pause");
        // } 
        if(iResult==SOCKET_ERROR){
            if(WSAGetLastError()==WSAECONNABORTED){
                cout << "Cliente desconectado por inactividad" << endl;
                tiempo_espera_se_agoto = true;
                system("pause");
            }
        }
        
        memset(buffer, 0, sizeof(buffer)); // reinicia buffer
        return respuesta_cliente;
    }

    void CerrarSocket()
    {
        closesocket(server);
        WSACleanup();
    }
};

void menu(Client *&cliente)
{
    int opcion = 0;
    string tamanio = "";
    string calculo = "";

    system("cls");
   
    cout << "Ingrese una opcion" << endl;
    cout << "1- Realizar calculo " << endl;
    cout << "2- Ver registro de actividades" << endl;
    cout << "3- Cerrar sesion " << endl;
    cin >> opcion;


    system("cls");
    switch (opcion)
    {
    case 1:
        cout << "/Si desea volver al menu anterior ingrese la palabra volver/" << endl;
        cout << "Ingrese la operacion: " << endl;
        cin.ignore();
        getline(cin, calculo);
        if (calculo == "volver")
        {
            menu(*&cliente);
        }
        cliente->Enviar("1" + calculo);
        cout << "Resultado: " << cliente->Recibir() << endl;
        if(tiempo_espera_se_agoto == false){system("pause");menu(*&cliente);}
        break;
    case 2:
        cliente->Enviar("2");
        tamanio = cliente->Recibir();
        cliente->Enviar("2");
        if(tiempo_espera_se_agoto == false){
            for (int i = 0; i < stoi(tamanio); i++)
            {
                cout<<cliente->Recibir()<<endl;
                cliente->Enviar("");
                
            }
            cliente->Recibir();
        system("pause");
        menu(*&cliente);
        }
        break;
    case 3:
        cliente->Enviar("3");
        cliente->Recibir();
        cliente->CerrarSocket();
        break;
    default:
        cout <<"Opcion incorrecta" << endl;
        system("pause");
        menu(*&cliente);
        break;
    }
    system("cls");
    
}

int main()
{
    Client *cliente = new Client();
    menu(cliente);
    return 0;
}
