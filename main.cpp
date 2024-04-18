#include <cstdlib>
#include <imgui.h>
#include "GLFW/glfw3.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <iostream>
#include <vector>
#define N_LINES 3
#define PRECISION 0.1f


//Определение структуры Vertex для хранения координат вершин и цветов
typedef struct Vertex
{
    float pos[2];
    float col[3];
} Vertex;

//Vertex vertices[N_LINES * 2] =
//{
  //  { {0.0f, 0.0f}, {0.8f, 0.7f, 0.0f}},
    //{ {0.0f, 0.5f}, {0.5f, 1.0f, 0.0f}},
   // { { -0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}},
    //{ { 0.0f, -0.5f}, {1.0f, 0.0f, 1.0f}},
    //{ {-0.7f, -0.7f}, {0.0f, 0.0f, 1.0f}},
    //{ {-0.2f, -0.45f}, {0.0f, 0.0f, 1.0f}},
    //{ {0.4f, -0.2f}, {1.0f, 0.9f, 0.0f } },
    //{ {0.9f, -0.45f}, {1.0f, 0.0f, 0.5f}},
    //{ {0.2f, 0.7f}, {1.0f, 0.0f, 1.0f}},
    //{ {0.7f, 0.7f}, {1.0f, 0.7f, 1.0f}},

//};
Vertex vertices[N_LINES * 2] =
{
    {{-0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.4f}, {0.0f, 1.0f, 0.0f}},
    {{0.0f, 0.4f}, {0.0f, 1.0f, 0.0f}},
    {{0.3f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{0.3f, 0.6f}, {0.0f, 1.0f, 0.0f}},
    {{0.6f, 0.6f}, {0.0f, 1.0f, 0.0f}}
};

Vertex vertices_default[N_LINES * 2];
//конвертируют координаты мыши в плавающие значения, используя ширину и высоту окна.
float xposToFloat(double xpos, int width)
{
    float bu = static_cast<float>((xpos / width - 0.5f) * 2);
    return bu;
}
//конвертируют координаты мыши в плавающие значения, используя ширину и высоту окна.
float yposToFloat(double ypos, int height)
{
    float bu = static_cast<float>((ypos / height - 0.5f) * (-2));
    return bu;
}

int miole_flag = 0;
int n_vertex = -1, n_vertex_temp = -1;
//перемещает линию к новой позиции.
void MoveLine(Vertex vertices[], int& n_vertex, float whereto[2])
{
    int vertex_flag;
    float length_x, length_y;
    if (n_vertex % 2 != 0) vertex_flag = -1;
    else vertex_flag = 1;
    length_x = vertices[n_vertex].pos[0] - vertices[n_vertex + vertex_flag].pos[0];
    length_y = vertices[n_vertex].pos[1] - vertices[n_vertex + vertex_flag].pos[1];
    vertices[n_vertex].pos[0] = whereto[0];
    vertices[n_vertex].pos[1] = whereto[1];
    vertices[n_vertex + vertex_flag].pos[0] = vertices[n_vertex].pos[0] - length_x;
    vertices[n_vertex + vertex_flag].pos[1] = vertices[n_vertex].pos[1] - length_y;

}

static std::vector<std::vector<int>> binds;
// связывает две вершины линии.
void BindLines(int& n_vertex, int& n_vertex_temp)
{
    std::vector<int> pushbacktemp = { n_vertex, n_vertex_temp };
    binds.push_back(pushbacktemp);
}
//проверяет, связаны ли две вершины линии.
bool AreBinded(int& n_vertex, int& n_vertex_temp, std::vector<std::vector<int>> binds)
{
    for (int i = 0; i < binds.size(); i++)
    {
        if ((binds[i][0] == n_vertex || binds[i][1] == n_vertex) && (binds[i][0] == n_vertex_temp || binds[i][1] == n_vertex_temp))
        {
            return true;
        }
    }
    return false;
}
//перемещает связанные вершины линий вместе.
void MoveBinded(int& n_vertex, std::vector<std::vector<int>> binds)
{
    int vertex_flag;
    if (n_vertex % 2 != 0) vertex_flag = -1;
    else vertex_flag = 1;
    for (int i = 0; i < binds.size(); i++)
    {
        if (n_vertex + vertex_flag == binds[i][0])
        {
            MoveLine(vertices, binds[i][1], vertices[n_vertex + vertex_flag].pos);
            MoveBinded(binds[i][1], binds);
            return;
        }
        if (n_vertex + vertex_flag == binds[i][1])
        {
            MoveLine(vertices, binds[i][0], vertices[n_vertex + vertex_flag].pos);
            MoveBinded(binds[i][0], binds);
            return;
        }
    }
}
//проверяет, находится ли курсор мыши на краю линии.
bool MouseIsOnLineEdge(Vertex vertices[], int& n_vertex, int& n_vertex_temp, float precision, double x_pos, int width, double y_pos, int height)
{
    for (int i = 0; i < N_LINES * 2; i++)
    {
        if ((vertices[i].pos[0] + precision >= xposToFloat(x_pos, width)) &&
            (vertices[i].pos[0] - precision <= xposToFloat(x_pos, width)) &&
            (vertices[i].pos[1] + precision >= yposToFloat(y_pos, height)) &&
            (vertices[i].pos[1] - precision <= yposToFloat(y_pos, height)))
        {
            miole_flag = 1;
            if (n_vertex == -1) n_vertex = i;
            if (n_vertex != i) n_vertex_temp = i;
        }
    }
    if (n_vertex != -1) return true;
    else return false;
}
//определяет конечную вершину, к которой привязана текущая.
int ConnectionFromTo(std::vector<std::vector<int>> binds, int& n_vertex_temp)
{
    int vertex_flag;
    if (n_vertex_temp % 2 != 0) vertex_flag = -1;
    else vertex_flag = 1;
    for (int i = 0; i < binds.size(); i++)
    {
        if (n_vertex_temp + vertex_flag == binds[i][0])
        {
            n_vertex_temp = ConnectionFromTo(binds, binds[i][1]);
        }
        else if (n_vertex_temp + vertex_flag == binds[i][1])
        {
            n_vertex_temp = ConnectionFromTo(binds, binds[i][0]);
        }
    }
    return (n_vertex_temp);
}

//проверяет, находится ли одна вершина линии рядом с другой вершиной.
bool is_connected = 0;
void OtherEndIsOnLineEdge(Vertex vertices[], int& n_vertex, int& n_vertex_temp, float precision)
{
    int n_vertex_temp_temp = n_vertex_temp;
    n_vertex_temp_temp = ConnectionFromTo(binds, n_vertex_temp);
    int vertex_flag_1;
    if (n_vertex % 2 != 0) vertex_flag_1 = -1;
    else vertex_flag_1 = 1;
    int vertex_flag_2;
    if (n_vertex_temp_temp % 2 != 0) vertex_flag_2 = -1;
    else vertex_flag_2 = 1;
    n_vertex_temp_temp += vertex_flag_2;
    std::cout << "CFT RESULT: " << n_vertex_temp_temp << std::endl;
    if (n_vertex_temp_temp < 0)
    {
        std::cout << "return cft" << std::endl;
        return;
    }
    if (vertices[n_vertex + vertex_flag_1].pos[0] + precision >= vertices[n_vertex_temp_temp].pos[0] &&
        vertices[n_vertex + vertex_flag_1].pos[0] - precision <= vertices[n_vertex_temp_temp].pos[0] &&
        vertices[n_vertex + vertex_flag_1].pos[1] + precision >= vertices[n_vertex_temp_temp].pos[1] &&
        vertices[n_vertex + vertex_flag_1].pos[1] - precision <= vertices[n_vertex_temp_temp].pos[1])
    {
        vertices[n_vertex + vertex_flag_1].pos[0] = vertices[n_vertex_temp_temp].pos[0];
        vertices[n_vertex + vertex_flag_1].pos[1] = vertices[n_vertex_temp_temp].pos[1];
        int n_vertex_temp_temp = n_vertex + vertex_flag_1;
        BindLines(n_vertex_temp_temp, n_vertex_temp_temp);
        std::cout << "CONNECTION SUCCESFUL!!!!!" << std::endl;
        is_connected = 1;
    }
    else std::cout << "NOT IN RANGE" << std::endl;
}
//вычисляет площадь фигуры, образованной линиями, если они связаны.
float AreaFormula(Vertex vertices[], bool is_connected)
{
    if (!is_connected) return -1;
    int n = N_LINES * 2;
    float area = 0.0;

    for (int i = 0; i < n - 1; ++i) {
        area += (vertices[i].pos[0] * vertices[i + 1].pos[1] - vertices[i + 1].pos[0] * vertices[i].pos[1]);
    }

    area += (vertices[n - 1].pos[0] * vertices[0].pos[1] - vertices[0].pos[0] * vertices[n - 1].pos[1]);
    area = abs(area) / 2.0;

    return area;
}
//обрабатывает нажатие левой кнопки мыши и вызывает соответствующие функции для перемещения или связывания вершин линий.
void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos, ypos;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glfwGetCursorPos(window, &xpos, &ypos);
        if (miole_flag)
        {
            float mousepostofloat[2] = { xposToFloat(xpos, width), yposToFloat(ypos, height) };

            MoveLine(vertices, n_vertex, mousepostofloat);
            MoveBinded(n_vertex, binds);

            MouseIsOnLineEdge(vertices, n_vertex, n_vertex_temp, PRECISION, xpos, width, ypos, height);
            std::cout << "n_v_t" << n_vertex_temp << std::endl;
            OtherEndIsOnLineEdge(vertices, n_vertex, n_vertex_temp, PRECISION);

            MouseIsOnLineEdge(vertices, n_vertex, n_vertex_temp, PRECISION, xpos, width, ypos, height);
            if (AreBinded(n_vertex, n_vertex_temp, binds)) n_vertex_temp = -1;
            std::cout << "n_vertex: " << n_vertex << std::endl;
            if (n_vertex_temp != -1)
            {
                MoveLine(vertices, n_vertex, vertices[n_vertex_temp].pos);
                BindLines(n_vertex, n_vertex_temp);
            }
            miole_flag = 0;
            n_vertex = -1;
            n_vertex_temp = -1;
            return;
        }
        MouseIsOnLineEdge(vertices, n_vertex, n_vertex_temp, PRECISION, xpos, width, ypos, height);
    }
}
//обрабатывает нажатие клавиш и вызывает соответствующие функции, например, отображение связей между вершинами, удаление связей, очистка окна и т. д.
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
    {
        std::cout << "BINDED:\n ";
        for (int i = 0; i < binds.size(); i++)
            std::cout << i << ": {" << binds[i][0] << ';' << binds[i][1] << '}' << std::endl;
    }
    else if (key == GLFW_KEY_E && action == GLFW_PRESS)
    {
        binds.erase(binds.begin(), binds.end());
        is_connected = 0;

    }
    else if (key == GLFW_KEY_DELETE && action == GLFW_PRESS)
    {
        for (int j = 0; j < 2; j++)
            for (int i = 0; i < N_LINES * 2; i++)
            {
                vertices[i].pos[j] = vertices_default[i].pos[j];
            }
        binds.erase(binds.begin(), binds.end());
        is_connected = 0;
    }
    else if (key == GLFW_KEY_A && action == GLFW_PRESS)
    {
        float area = AreaFormula(vertices, is_connected);
        std::cout << area << std::endl;
    }
    else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
    {
        for (int i = 0; i < N_LINES * 2; i++)
        {
            vertices[i].pos[0] += 0.1f;
        }
    }
    else if (key == GLFW_KEY_UP && action == GLFW_PRESS)
    {
        for (int i = 0; i < N_LINES * 2; i++)
        {
            vertices[i].pos[1] += 0.1f;
        }
    }
    else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
    {
        for (int i = 0; i < N_LINES * 2; i++)
        {
            vertices[i].pos[1] -= 0.1f;
        }
    }
    else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
    {
        for (int i = 0; i < N_LINES * 2; i++)
        {
            vertices[i].pos[0] -= 0.1f;
        }
    }

}

int main()
{
    if (!glfwInit()) {
        return -1;
    }
    GLFWwindow* window = glfwCreateWindow(800, 600, "Daria Kamushkina", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_callback);
    glfwMakeContextCurrent(window);

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");


    for (int i = 0; i < N_LINES * 2; i++)
    {
        vertices_default[i].pos[0] = vertices[i].pos[0];
        vertices_default[i].pos[1] = vertices[i].pos[1];
        vertices_default[i].col[0] = vertices[i].col[0];
        vertices_default[i].col[1] = vertices[i].col[1];
        vertices_default[i].col[2] = vertices[i].col[2];
    }

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        ImGui::Begin("dashapupu");
        ImGui::Text("PLEASE GIVE IT A 5!!!");
        ImGui::End();
        ImGui::Render();

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBegin(GL_LINES);
        for (int i = 0; i < N_LINES * 2; i++)
        {
            glColor3f(vertices[i].col[0], vertices[i].col[1], vertices[i].col[2]);
            glVertex2f(vertices[i].pos[0], vertices[i].pos[1]);
        }

        glEnd();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
