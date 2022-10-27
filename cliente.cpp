#include <iostream>
#include <winsock2.h>
#include <conio.h>
#include <string>
#include <algorithm>
#include <clocale>//es para usar � y acento
#include <ctime>//para comparar la fecha con la fecha actual
#include <cstdlib>
#include <vector>


using namespace std;

vector<string> verificarIpYPuerto(){

    std::string ipInput="127.0.0.1";
    std::string puertoInput = "5000";
    vector<string> datos;

    // while(ipInput==""){
    //     cout<<"Ingrese la direccion IP: ";
    //     cin>>ipInput;
    //     system("CLS");
    // }

    // while(puertoInput==""){
    //     cout<<"Ingrese el puerto: ";
    //     cin>>puertoInput;
    //     system("CLS");
    // }


    datos.push_back(ipInput);
    datos.push_back(puertoInput);

    return datos;
}


class Client{
public:
    WSADATA WSAData;
    SOCKET server;
    SOCKADDR_IN addr;
    char buffer[1024];
    Client()
    {
        cout<<"Conectando al servidor..."<<endl<<endl;

        WSAStartup(MAKEWORD(2,0), &WSAData);
        server = socket(AF_INET, SOCK_STREAM, 0);

        vector<string> datos = verificarIpYPuerto();

        string s = datos[0];
        int n = s.length();
        char ip[n + 1];
        strcpy(ip, s.c_str());

        addr.sin_addr.s_addr = inet_addr(ip);

        addr.sin_family = AF_INET;

        addr.sin_port = htons(stol(datos[1]));

        int crespuesta = connect(server, (SOCKADDR *)&addr, sizeof(addr));
        if(crespuesta==0){
            cout << "Conectado al Servidor!" << endl;
        }
        else{
            cout<<"Error al conectarse. Vuelva a intentarlo mas tarde."<<endl;
            CerrarSocket();
            system("pause");
            exit(1);
        }

    }
    void Enviar(string respuesta)
    {
        for(int i=0;i<respuesta.length();i++){
            this->buffer[i]= respuesta[i];
        }

        send(server, buffer, sizeof(buffer), 0);
        memset(buffer, 0, sizeof(buffer));

    }

    string Recibir()
    {
        recv(server, buffer, sizeof(buffer), 0);

        string buff = buffer;

        if(buff=="Timeout"){
            cout<<"Sesion expirada"<<endl;
            CerrarSocket();
        }
        memset(buffer, 0, sizeof(buffer));
        return buff;
    }

    void CerrarSocket()
    {
       closesocket(server);
       WSACleanup();
       cout << "Socket cerrado." << endl << endl;
    }
};

string solicitarCalculo(){
    string calculo;
    cout<<"/Si desea volver al menu anterior ingrese la letra s/"<<endl;
    cout<<"Ingrese la operacion: "<<endl;
    //cin>>calculo;
    cin.ignore();
    getline(cin,calculo);
    cout<<calculo<<endl;
    return calculo;
}

void menu(Client *&cliente)
{
    //contador de 2 minutos
    int opcion;

    cout<<"Ingrese una opcion"<<endl;
    cout<<"1- Realizar calculo "<<endl;
    cout<<"2- Ver registro de actividades"<<endl;
    cout<<"3- Cerrar sesion "<<endl;
    cin>>opcion;
   
    switch(opcion)
    {
        case 1:cliente->Enviar("1"+solicitarCalculo());break;
        case 2:cliente->Enviar("2");break;
        case 3:cliente->CerrarSocket();break;
        default:cout<<"Opcion incorrecta";break;//tiene que volver a preguntar
    }

    string resultado=cliente->Recibir();
    cout<<"Respuesta Servidor: "<<resultado<<endl;
}


/***********************************************************************/
/********************************MAIN***********************************/
/***********************************************************************/
int main()
{
    setlocale(LC_CTYPE,"Spanish");// Spanish (de la librer�a locale.h) es para usar � y acento
    string var;
    Client *cliente = new Client();
    menu(cliente);
    return 0;
}
/***********************************************************************/
/*******************************FIN MAIN********************************/
/***********************************************************************/