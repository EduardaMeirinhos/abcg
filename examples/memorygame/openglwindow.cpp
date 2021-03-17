#include "openglwindow.hpp"

#include <fmt/core.h>
#include <imgui.h>

#include <array>
#include <cppitertools/itertools.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <random>
#include <string>

void OpenGLWindow::initializeGL() {
  const auto* vertexShader{R"gl(
    #version 410

    layout(location = 0) in vec2 inPosition;
    layout(location = 1) in vec4 inColor;

    out vec4 fragColor;

    void main() {
      gl_Position = vec4(inPosition, 0, 1);
      fragColor = inColor;
    }
  )gl"};

  const auto* fragmentShader{R"gl(
    #version 410

    in vec4 fragColor;

    out vec4 outColor;

    void main() { outColor = fragColor; }
  )gl"};

  // Create shader program
  m_program = createProgramFromString(vertexShader, fragmentShader);

  // Clear window
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  restart(GameDificulty::Easy); // Starts the game with Easy mode
}

void OpenGLWindow::paintGL() {
  glViewport(0, 0, m_viewportWidth, m_viewportHeight);
  update();
  glClear(GL_COLOR_BUFFER_BIT);
  glUseProgram(m_program);
  for (int i = 0; i < m_N * m_N; i++) {
    Card* cd = &(cards[i]);
    glBindVertexArray(cd->m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
  }
  glUseProgram(0);
}

void OpenGLWindow::paintUI() {
  auto appWindowWidth{getWindowSettings().width};
  auto appWindowHeight{getWindowSettings().height};

  {
    // Header window
    auto windowWidth{appWindowWidth * 1.0f};
    auto windowHeight{80};
    ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));
    ImGui::SetNextWindowPos(ImVec2((appWindowWidth - windowWidth) / 2, 0));
    auto flags{ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize |
               ImGuiWindowFlags_NoCollapse};
    ImGui::Begin("Jogo da Memória", nullptr, flags);
    {
      bool easyRestartSelected{};
      bool mediumRestartSelected{};
      bool hardRestartSelected{};
      // Game menu
      if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Novo Jogo")) {
          ImGui::MenuItem("Fácil", nullptr, &easyRestartSelected);
          ImGui::MenuItem("Médio", nullptr, &mediumRestartSelected);
          ImGui::MenuItem("Difícil", nullptr, &hardRestartSelected);
          ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
      }
      if (easyRestartSelected) {
        timer.restart();
        restart(GameDificulty::Easy);
      }
      if (mediumRestartSelected) {
        timer.restart();
        restart(GameDificulty::Medium);
      }
      if (hardRestartSelected) {
        timer.restart();
        restart(GameDificulty::Hard);
      }

      // Show message
      std::string text;
      switch (m_gameState) {
        case GameState::Starting:
          text = "Memorize!";
          break;
        case GameState::Play:
          text = fmt::format("Tempo decorrido: {}", convertTimeSeconds());
          break;
        case GameState::Waiting:
          text = fmt::format("Tempo decorrido: {}", convertTimeSeconds());
          break;
        case GameState::WinPlayer:
          text = fmt::format("Parabéns! Seu tempo: {}", elapsedTimeWin);
          break;
      }

      ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize(text.c_str()).x) /
                           2);
      ImGui::Text("%s", text.c_str());

      ImGui::End();
    }

    // Grid Window
    windowWidth = appWindowWidth * 1.0f;
    windowHeight = (appWindowHeight * 1.0f) - 80;
    ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));
    ImGui::SetNextWindowPos(ImVec2((appWindowWidth - windowWidth) / 2, 80));
    flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration;
    ImGui::Begin("Grid", nullptr, flags);

    // Menu
    {
      // Create game board
      auto gridHeight{windowHeight - 55};

      // For each row
      for (auto i : iter::range(m_N)) {
        ImGui::Columns(m_N, NULL, false);
        // For each column
        for (auto j : iter::range(m_N)) {
          auto offset{i * m_N + j};
          // Renderizes buttons
          if (cards[offset].showing != 0) {
            ImGui::InvisibleButton("", ImVec2(-1, gridHeight / m_N));
          } else {
            ImGui::PushStyleColor(ImGuiCol_Button,
                                  ImVec4(ImColor(100, 100, 100, 255)));
            ImGui::Button("", ImVec2(-1, gridHeight / m_N));
            ImGui::PopStyleColor();
          }
          if (m_gameState == GameState::Play && cards[offset].showing != 2) {
            if (ImGui::IsItemClicked()) {
              cards[offset].showing = 1;
              qntShowing++;
              checkEquals();
              checkWin();
            }
          }
          ImGui::NextColumn();
        }
      }
      ImGui::End();
    }
  }
}

// Cheack if cards are equal
void OpenGLWindow::checkEquals() {
  char value1{' '};
  char value2{' '};
  if (qntShowing == 2) {
    for (int i = 0; i < m_N * m_N; i++) {
      if (cards[i].showing == 1) {
        if (value1 == ' ') {
          value1 = cards[i].value;
        } else {
          value2 = cards[i].value;
          break;
        }
      }
    }
    if (value1 != value2) {
      timer.restart();
      m_gameState = GameState::Waiting;
    } else {
      for (int i = 0; i < m_N * m_N; i++) {
        if (cards[i].value == value1) {
          cards[i].showing = 2;
        }
      }
    }
    qntShowing = 0;
  }
}

// Check if the player won
void OpenGLWindow::checkWin() {
  for (int i = 0; i < m_N * m_N; i++) {
    if (cards[i].showing != 2) {
      return;
    }
  }
  m_gameState = GameState::WinPlayer;
  elapsedTimeWin = convertTimeSeconds();
}

// Controls when cards are hidden  
void OpenGLWindow::update() {
  // Controls when cards are hidden (two different cards have been chosen)
  if (m_gameState == GameState::Waiting && timer.elapsed() > 1) {
    m_gameState = GameState::Play;
    for (int i = 0; i < m_N * m_N; i++) {
      if (cards[i].showing != 2) {
        cards[i].showing = 0;
      }
    }
  }
  // Controls when ALL the cards are hidden in the beggining of the game
  if (m_gameState == GameState::Starting && timer.elapsed() > 3) {
    m_gameState = GameState::Play;
    for (int i = 0; i < m_N * m_N; i++) {
      cards[i].showing = 0;
    }
    universalTimer.restart();
  }
  for (int i = 0; i < m_N * m_N; i++) {
    Card* cd;
    cd = &(cards[i]);
    createTriangle(cd, i);
  }
}

// Start new game
void OpenGLWindow::restart(GameDificulty dificulty) {
  m_gameState = GameState::Starting;
  qntShowing = 0;
  m_N = 4;
  int values[] = {0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7};
  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(values, values + m_N * m_N, g);

  float colors[8][9];
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 9; j++) {
      colors[i][j] = ((double)rand() / (RAND_MAX));
    }
  }

  if (dificulty == GameDificulty::Easy) {
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 3; j++) {
        colors[i][j + 3] = colors[i][j];
        colors[i][j + 6] = colors[i][j];
      }
    }
  } else if (dificulty == GameDificulty::Medium) {
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 3; j++) {
        colors[i][j + 3] = colors[i][j];
      }
    }
  }

  for (int i = 0; i < m_N * m_N; i++) {
    Card cd;
    cd.value = values[i];
    cd.showing = 1;
    for (int i = 0; i < 9; i++) {
      cd.colors[i] = colors[cd.value][i];
    }
    cards[i] = cd;
  }
  for (int i = 0; i < m_N * m_N; i++) {
    Card* cd;
    cd = &(cards[i]);
    createTriangle(cd, i);
  }
}

// Convert seconds to hours, minutes, seconds
std::string OpenGLWindow::convertTimeSeconds() {
  // Initialize cards
  std::string text;
  int now = (int)universalTimer.elapsed();
  int hours = now / (60 * 60);
  now -= hours * (60 * 60);
  int minutes = now / (60);
  now -= minutes * (60);
  int seconds = now;
  text = fmt::format("{}:{}:{}", hours, minutes, seconds);
  return text;
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::terminateGL() {
  glDeleteProgram(m_program);
  for (int i = 0; i <= m_N * m_N; i++) {
    Card* cd = &(cards[i]);
    glDeleteBuffers(1, &(cd->m_vboPositions));
    glDeleteBuffers(1, &(cd->m_vboColors));
    glDeleteVertexArrays(1, &(cd->m_vao));
  }
}

// Triangle definitions (position, color, vbo, vao...)
void OpenGLWindow::createTriangle(Card* cd, int pos) {
  glDeleteBuffers(1, &(cd->m_vboPositions));
  glDeleteBuffers(1, &(cd->m_vboColors));
  glDeleteVertexArrays(1, &(cd->m_vao));

  float header = 2.0 * (110.0 / getWindowSettings().height);
  float footer = 2.0 * (15.0 / getWindowSettings().height);
  float sep = 2.0 * (4.0 / getWindowSettings().height);
  float buttonHeight = (2.0 - header - footer) / 4.0 - sep;

  int x_add = pos % 4;
  int y_add = (int)pos / 4;

  float x1 = -1.0 + 0.5 * x_add + 0.075;
  float y1 = 1.0 - header - (y_add + 1.0) * (buttonHeight + sep) + 0.05;
  float x2 = -0.75 + 0.5 * x_add;
  float y2 = 1.0 - header - (y_add) * (buttonHeight + sep) - 0.05;
  float x3 = -0.5 + 0.5 * x_add - 0.075;
  float y3 = 1.0 - header - (y_add + 1.0) * (buttonHeight + sep) + 0.05;

  // Create vertex positions
  std::array positions{glm::vec2(x1, y1), glm::vec2(x2, y2), glm::vec2(x3, y3)};

  std::vector<glm::vec4> colors(0);
  // Create vertex colors
  if (cd->showing != 0) {
    colors.push_back(
        glm::vec4{cd->colors[0], cd->colors[1], cd->colors[2], 1.0f});
    colors.push_back(
        glm::vec4{cd->colors[3], cd->colors[4], cd->colors[5], 1.0f});
    colors.push_back(
        glm::vec4{cd->colors[6], cd->colors[7], cd->colors[8], 1.0f});
  } else {
    colors.push_back(glm::vec4{0.39f, 0.39f, 0.39f, 1.0f});
    colors.push_back(glm::vec4{0.39f, 0.39f, 0.39f, 1.0f});
    colors.push_back(glm::vec4{0.39f, 0.39f, 0.39f, 1.0f});
  }

  // Generate VBO of positions
  glGenBuffers(1, &(cd->m_vboPositions));
  glBindBuffer(GL_ARRAY_BUFFER, cd->m_vboPositions);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions.data(),
               GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate VBO of colors
  glGenBuffers(1, &(cd->m_vboColors));
  glBindBuffer(GL_ARRAY_BUFFER, cd->m_vboColors);
  glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec4),
               colors.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};
  GLint colorAttribute{glGetAttribLocation(m_program, "inColor")};

  // Create VAO
  glGenVertexArrays(1, &(cd->m_vao));

  // Bind vertex attributes to current VAO
  glBindVertexArray(cd->m_vao);

  glEnableVertexAttribArray(positionAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, cd->m_vboPositions);
  glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glEnableVertexAttribArray(colorAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, cd->m_vboColors);
  glVertexAttribPointer(colorAttribute, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  glBindVertexArray(0);
}