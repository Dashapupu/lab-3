# GladExample
Базовое приложение glfw+glad+ImGui с применением шейдеров. Вращение треугольника. Изменение цвета

#Сборка под консоль Windows

cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug .. -DCMAKE_CXX_FLAGS="-mwindows"

cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release .. -DCMAKE_CXX_FLAGS="-mwindows"

cmake --build . -j16
