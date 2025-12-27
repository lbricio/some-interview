# RickMortyMiddleware
> Essa aplicação é um middleware que consome a API Rest [rickandmortyapi](https://rickandmortyapi.com/documentation/#get-multiple-characters).
> A aplicaçao também possui uma camada de cache local que armazena personagens, locais e episódios em modelos locais evitando overload excessivo na API consumida. Utiliza Conan2 como gerenciador de pacotes, CMake para automatizar o build, C++ como linguagem e Boost(Asio, Beast, JSON) como framework principal e Google Test (GTest) para testes automatizados, além disso o projeto aplica boas práticas de divisão de responsabilidades e modularização de código.

## Endpoints
`GET /help` visualiza todos os endpoints disponíveis
  
`GET /character/all`       retorna todos os personsagens em um único json;  
`GET /character/<id>`      retorna um personagem específico pelo id;  
`GET /character/<id>,<id>` retorna vários personagens especificados por id;  
`GET /character/<?query>`  retorna personagens que cumprem o filtro especificado;  
  
`GET /episode/all`         retorna todos os episódios em um único json;  
`GET /episode/<id>`        retorna um episódio específico pelo id;  
`GET /episode/<id>,<id>`   retorna vários episódios por id;  
`GET /episode/<?query>`    retorna episódios a partir do filtro especificado; 
  
`GET /location/all`       retorna todas as localizações em um único json;  
`GET /location/<id>`      retorna uma localização especificada pelo id;  
`GET /location/<id>,<id>` retorna várias localizações especificadas por id;  
`GET /location/<?query>`  retorna localizações a partir do filtro especificado;  

## Stack
| Tecnologia                          |  Descrição                                        |
| ----------------------------------- | ------------------------------------------------- |
| `C++20`                           | Linguagem principal do desafio                    |
| `Boost/Asio`                      | Networking (HTTP/HTTPS client + server)           |
| `Boost/Beast`                     | Engine HTTP e abstração de streams                |
| `Boost/JSON`                      | Parse e serialização de JSON                      |
| `GTest`                           | Testes unitários automatizados                    |
| `CMake`                           | Build system e automação de testes                |
| `Conan`                           | Gerenciador de dependências/pacotes               |
| `CMakeLists.txt`                  | Orquestra compilação e execução do test suite     |

## Estrutura do Projeto
```
📁 RickMortyMiddleware
├── 📁 include  
│   ├── api.hpp            Declara API do middleware + cache
│   ├── http_client.hpp    Interface do cliente HTTPS externo
│   ├── handler.hpp        Router/Handling services
│   ├── models.hpp         Modelos do domínio (Character, Episode e Location)
│   └── utils.hpp          Funções auxiliares
│  
├── 📁 src  
│   ├── api.cpp            Implementa consumo API externa + cache
│   ├── http_client.cpp    Implementa HTTPS para camada de transporte
│   ├── router.cpp         Roteia os endpoints para os handlers
│   ├── handler.cpp        Faz o processamento das requests
│   └── utils.cpp          Funções auxiliares
│  
├── 📁 tests  
│   ├── test_main.cpp      Inicializa GTest + testes unitários
│   └── test_endpoint.cpp  Testes de integração dos endpoints
│  
├── CMakeLists.txt         Orquestrador do build
├── conanfile.txt          Manifesto de dependências
└── CMakePresets.json      Configurações do CMake
```

---

## Install

#### Package Manager

Instalação do Conan (caso não esteja disponível)
```shell
pip3 install --upgrade conan
conan profile detect
```

Verifique a instalação:
```shell
conan --version
```
#### Build

1. Instalar dependências com Conan:
```shell
conan install . --output-folder=build --build=missing -s build_type=Release
```

2. Configurar o CMake usando o toolchain do Conan:
```shell
cmake -S . -B build/Release \
  -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake \
  -DCMAKE_BUILD_TYPE=Release
```

3. Compilar:
```shell
cmake --build build/Release
```

4. Executar o Middleware
```shell
./build/Release/app
```

5. Rodar testes
```shell
ctest --test-dir build/Release --output-on-failure
```
  
---
  
## Exemplos

Ao iniciar o Middleware ele passa a rodar na porta 8080 e lê ativamente requisções recebidas pelo client:  

```text
Middleware started at port 8080
```
  
Exemplo de requisição `/character/<id>`:  
  
```text
localhost:8080/character/12
```

Resposta:
```json
{
  "id": 12,
  "name": "Alexander",
  "status": "Dead",
  "species": "Human",
  "gender": "Male",
  "origin": "Earth (C-137)",
  "location": "Anatomy Park",
  "episodes": [3]
}
```
  
Exemplo de requisição `/episode/all` :
  
```text
localhost:8080/episode/all
```

Resposta:  
```json
{
  "info": {
    "count": 51,
    "pages": 3,
    "next": "https://rickandmortyapi.com/api/episode?page=2",
    "prev": null
  },
  "results": [
    {
      "id": 1,
      "name": "Pilot",
      "air_date": "December 2, 2013",
      "episode": "S01E01",
      "characters": [
        "https://rickandmortyapi.com/api/character/1",
        "https://rickandmortyapi.com/api/character/2",
        "https://rickandmortyapi.com/api/character/35",
        "https://rickandmortyapi.com/api/character/38",
        "https://rickandmortyapi.com/api/character/62",
        "https://rickandmortyapi.com/api/character/92",
        "https://rickandmortyapi.com/api/character/127",
        "https://rickandmortyapi.com/api/character/144",
        "https://rickandmortyapi.com/api/character/158",
        "https://rickandmortyapi.com/api/character/175",
        "https://rickandmortyapi.com/api/character/179",
        "https://rickandmortyapi.com/api/character/181",
        "https://rickandmortyapi.com/api/character/239",
        "https://rickandmortyapi.com/api/character/249",
        "https://rickandmortyapi.com/api/character/271",
        "https://rickandmortyapi.com/api/character/338",
        "https://rickandmortyapi.com/api/character/394",
        "https://rickandmortyapi.com/api/character/395",
        "https://rickandmortyapi.com/api/character/435"
      ],
      "url": "https://rickandmortyapi.com/api/episode/1",
      "created": "2017-11-10T12:56:33.798Z"
    }]
    "..."
}
```

Exemplo de requisição `/location/?name=earth` :
  
```text
localhost:8080/location/?name=earth
```

Resposta:  
```json
{
  "info": {
    "count": 126,
    "pages": 7,
    "next": "https://rickandmortyapi.com/api/location/?page=2",
    "prev": null
  },
  "results": [
    {
      "id": 1,
      "name": "Earth (C-137)",
      "type": "Planet",
      "dimension": "Dimension C-137",
      "residents": [
        "https://rickandmortyapi.com/api/character/38",
        "https://rickandmortyapi.com/api/character/45",
        "https://rickandmortyapi.com/api/character/71",
        "https://rickandmortyapi.com/api/character/82",
        "https://rickandmortyapi.com/api/character/83",
        "https://rickandmortyapi.com/api/character/92",
        "https://rickandmortyapi.com/api/character/112",
        "https://rickandmortyapi.com/api/character/114",
        "https://rickandmortyapi.com/api/character/116",
        "https://rickandmortyapi.com/api/character/117",
        "https://rickandmortyapi.com/api/character/120",
        "https://rickandmortyapi.com/api/character/127",
        "https://rickandmortyapi.com/api/character/155",
        "https://rickandmortyapi.com/api/character/169",
        "https://rickandmortyapi.com/api/character/175",
        "https://rickandmortyapi.com/api/character/179",
        "https://rickandmortyapi.com/api/character/186",
        "https://rickandmortyapi.com/api/character/201",
        "https://rickandmortyapi.com/api/character/216",
        "https://rickandmortyapi.com/api/character/239",
        "https://rickandmortyapi.com/api/character/271",
        "https://rickandmortyapi.com/api/character/302",
        "https://rickandmortyapi.com/api/character/303",
        "https://rickandmortyapi.com/api/character/338",
        "https://rickandmortyapi.com/api/character/343",
        "https://rickandmortyapi.com/api/character/356",
        "https://rickandmortyapi.com/api/character/394"
      ],
      "url": "https://rickandmortyapi.com/api/location/1",
      "created": "2017-11-10T12:42:04.162Z"
    }]
  "..."
}
```

