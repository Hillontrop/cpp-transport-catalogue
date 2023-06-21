# Транспортный справочник (Transport Сatalogue)

## 1. Описание
Это реализация транспортного справочника, который позволяет добавлять остановки, маршруты и расстояния между остановками, а также получать информацию о маршрутах и остановках. Имеет поддержку ввод-вывод в формате JSON и  визуализацию в формате SVG.

## 2. Цель проекта
Цель проекта "Транспортный справочник" заключается в создании эффективного инструмента для хранения и обработки информации о транспортных маршрутах и остановках. Главной целью проекта является предоставление пользователям удобного интерфейса для работы с данными о маршрутах общественного транспорта, а также возможности получать информацию о расстояниях между остановками и характеристиках маршрутов.

### Основные задачи проекта включают:

* **Хранение данных:** Создание структуры данных, позволяющей эффективно хранить информацию о маршрутах, остановках и расстояниях между ними.

* **Добавление информации:** Реализация функционала для добавления новых остановок и маршрутов, а также задания расстояний между остановками.

* **Поиск и получение информации:** Предоставление возможности поиска маршрутов и остановок по имени, а также получение информации о характеристиках маршрутов, таких как количество остановок, уникальные остановки, длина маршрута и кривизна.

* **Поддержка форматов ввода-вывода:** Реализация функционала для ввода-вывода данных в формате JSON, позволяющего загружать и сохранять информацию о маршрутах и остановках.

* **Визуализация:** Предоставление возможности визуализации маршрутов и остановок в формате SVG для наглядного отображения данных.

Цель проекта "Транспортный справочник" заключается в создании удобного инструмента, который поможет пользователям легко управлять информацией о маршрутах и остановках общественного транспорта, а также получать необходимую информацию для планирования поездок и ориентации в городе.

## 3. Использование

Пример использования Transport Сatalogue:

	  #include "transport_catalogue.h"
	  #include "json_reader.h"
	  #include "request_handler.h"
	  
	  #include <iostream>
	  #include <sstream>
	  
	  int main()
	  {
	      using namespace transport_guide;
	  
	      catalogue::TransportCatalogue catalogue;
	  
	      // Чтение входных данных из строки
	      std::string str = R"(
	          {
	              "base_request_": {
	                  "add_stops": [
	                      {
	                          "name": "Stop1",
	                          "coordinates": {
	                              "latitude": 55.751244,
	                              "longitude": 37.618423
	                          }
	                      },
	                      {
	                          "name": "Stop2",
	                          "coordinates": {
	                              "latitude": 55.753215,
	                              "longitude": 37.621548
	                          }
	                      }
	                  ],
	                  "add_buses": [
	                      {
	                          "name": "Bus1",
	                          "stops": [
	                              "Stop1",
	                              "Stop2"
	                          ],
	                          "is_roundtrip": true
	                      }
	                  ],
	                  "set_distances": [
	                      {
	                          "stop_from": "Stop1",
	                          "stop_to": "Stop2",
	                          "distance": 1000
	                      }
	                  ]
	              },
	              "stat_request_": {
	                  "bus": "Bus1",
	                  "stop": "Stop1",
	                  "route": [
	                      "Stop1",
	                      "Stop2"
	                  ]
	              }
	          }
	      )";
	
	    std::istringstream input(str);
	    DataRequests query = ReaderInputCatalogueUpdate(std::cin);
	    CraftCatalogue(catalogue, query.base_request_);
	    RequestHandler(catalogue, query.stat_request_);
	
	    return 0;
	}

### В этом примере происходит следующее:

1. Создается объект TransportCatalogue для хранения справочника транспорта.

2. Используется std::istringstream для чтения входных данных из строки str.

3. Входные данные представлены в формате JSON и содержат запросы на добавление остановок, маршрутов и расстояний между остановками, а также статистические запросы.

4. Функция ReaderInputCatalogueUpdate считывает и преобразует JSON данные в структуру DataRequests, которая содержит отдельные запросы.

5. Функция CraftCatalogue обрабатывает базовые запросы и добавляет остановки, маршруты и расстояния в справочник catalogue.

6. Функция RequestHandler обрабатывает статистические запросы и выводит результаты на экран.

## 4.Описание методов
### - void AddStop(const std::string& name_stop, const geo::Coordinates& coordinates)
Метод AddStop добавляет остановку в справочник. Принимает имя остановки и её координаты.

### - void AddBus(const std::string& name_bus, const std::vector<std::string>& stops, bool is_roundtrip)
Метод AddBus добавляет маршрут автобуса в справочник. Принимает имя маршрута, вектор остановок и флаг, указывающий на то, является ли маршрут кольцевым.

### - BusInfo GetBusInfo(const std::string& name_bus) const
Метод GetBusInfo возвращает информацию о маршруте автобуса. Принимает имя маршрута и возвращает структуру BusInfo с информацией о маршруте.

### - Stop* FindStop(const std::string& name_stop) const
Метод FindStop находит остановку по её имени и возвращает указатель на объект Stop. Если остановка не найдена, возвращает nullptr.

### - std::optional<TransportRouter::RouteInfo> FindRoute(const std::string& stop_from, const std::string& stop_to) const
Метод FindRoute находит маршрут от одной остановки до другой и возвращает информацию о маршруте в виде объекта RouteInfo. Если маршрут не найден, возвращает std::nullopt.

## 5. Планы по доработке проекта
1. **Улучшение производительности:**
Оптимизация алгоритмов и структур данных для обеспечения более быстрой работы справочника, особенно при больших объемах данных.

2. **Расширение функциональности:**
Добавление дополнительных возможностей, таких как поиск оптимального маршрута, расчет стоимости проезда, учет расписания и других параметров.

3. **Тестирование и отладка:**
Создание комплексной системы тестирования для проверки правильности работы справочника и выявления возможных ошибок и уязвимостей.

4. **Документация и комментарии:**
Дополнение кода комментариями и документацией, чтобы обеспечить понятность и удобство использования проекта другими разработчиками.

5. **Оптимизация памяти:**
Анализ и оптимизация использования памяти для уменьшения объема занимаемой памяти справочником.

6. **Исправление ошибок и улучшение устойчивости:**
Исправление обнаруженных ошибок, улучшение обработки ошибочных ситуаций и обеспечение стабильной работы справочника в различных условиях.
