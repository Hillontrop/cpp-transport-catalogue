#include "transport_catalogue.h"
#include "json_reader.h"
#include "request_handler.h"
#include "map_renderer.h"


#include <iostream>
#include <sstream>
#include <fstream>
#include <string>


int main()
{
    //setlocale(LC_ALL,"ru");
    //std::ifstream file("s10_final_opentest_1.txt"); // �������� ����� ��� ������
    //std::stringstream input;
    //if (file)
    //{
    //    input << file.rdbuf(); // ������ ����������� ����� � ����� input
    //    file.close(); // �������� �����
    //}
    //else
    //{
    //    std::cout << "������ ��� �������� �����." << std::endl;
    //    return 1;
    //}

//    	std::string str = R"()";
//    	std::istringstream input(str);


	using namespace transport_guide;

	catalogue::TransportCatalogue catalogue;

	DataRequests query = ReaderInputCatalogueUpdate(std::cin);
	CraftCatalogue(catalogue, query.base_request_);
    std::string map_buses_and_stopes = MapRenderer(catalogue, query.parametr_);
	RequestHandler(catalogue, query.stat_request_,map_buses_and_stopes);
	return 0;
}
