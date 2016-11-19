cmake_minimum_required(VERSION 3.2.2)
project(simple_chat_server)							# Название проекта
add_subdirectory(service)							# Добавление подпроекта, указывается имя директории
add_subdirectory(server)							# Добавление подпроекта, указывается имя директории
add_subdirectory(client)							# Добавление подпроекта, указывается имя директории
