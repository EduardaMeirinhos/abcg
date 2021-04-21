#include "openglwindow.hpp"

#include <imgui.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <fmt/core.h>
#include <string> 

void OpenGLWindow::handleEvent(SDL_Event& ev) {
  if (ev.type == SDL_KEYDOWN) {
    if (ev.key.keysym.sym == SDLK_UP || ev.key.keysym.sym == SDLK_w)
      m_dollySpeed = 0.3f;
    if (ev.key.keysym.sym == SDLK_DOWN || ev.key.keysym.sym == SDLK_s)
      m_dollySpeed = -0.3f;
    if (ev.key.keysym.sym == SDLK_LEFT || ev.key.keysym.sym == SDLK_a)
      m_panSpeed = -0.5f;
    if (ev.key.keysym.sym == SDLK_RIGHT || ev.key.keysym.sym == SDLK_d)
      m_panSpeed = 0.5f;
    if (ev.key.keysym.sym == SDLK_q) m_truckSpeed = -0.3f;
    if (ev.key.keysym.sym == SDLK_e) m_truckSpeed = 0.3f;
    if (ev.key.keysym.sym == SDLK_r) m_liftSpeed = -0.3f;
    if (ev.key.keysym.sym == SDLK_f) m_liftSpeed = 0.3f;
  }
  if (ev.type == SDL_KEYUP) {
    if ((ev.key.keysym.sym == SDLK_UP || ev.key.keysym.sym == SDLK_w) &&
        m_dollySpeed > 0)
      m_dollySpeed = 0.0f;
    if ((ev.key.keysym.sym == SDLK_DOWN || ev.key.keysym.sym == SDLK_s) &&
        m_dollySpeed < 0)
      m_dollySpeed = 0.0f;
    if ((ev.key.keysym.sym == SDLK_LEFT || ev.key.keysym.sym == SDLK_a) &&
        m_panSpeed < 0)
      m_panSpeed = 0.0f;
    if ((ev.key.keysym.sym == SDLK_RIGHT || ev.key.keysym.sym == SDLK_d) &&
        m_panSpeed > 0)
      m_panSpeed = 0.0f;
    if (ev.key.keysym.sym == SDLK_q && m_truckSpeed < 0) m_truckSpeed = 0.0f;
    if (ev.key.keysym.sym == SDLK_e && m_truckSpeed > 0) m_truckSpeed = 0.0f;
    if (ev.key.keysym.sym == SDLK_r && m_liftSpeed < 0) m_liftSpeed = 0.0f;
    if (ev.key.keysym.sym == SDLK_f && m_liftSpeed > 0 ) m_liftSpeed = 0.0f;
  }
}

void OpenGLWindow::initializeGL() {
  glClearColor(0, 0, 0, 1);
  glEnable(GL_DEPTH_TEST);

  auto path{getAssetsPath() + "shaders/texture"};
  auto program{createProgramFromFile(path + ".vert", path + ".frag")};
  m_program = program;

  loadModel();
}

void OpenGLWindow::loadModel() {

  for (int i = 0; i < 10; i++){
    planets[i].m_model.loadFromFile(getAssetsPath() + filenames[i][0]);
    planets[i].m_model.loadDiffuseTexture(getAssetsPath() + "maps/" + filenames[i][1]);
    if (i == 6){
      planets[i].m_model.loadDiffuseTexture(getAssetsPath() + "maps/uranus_rings.map");
    }
    planets[i].m_model.setupVAO(m_program);
    planets[i].m_trianglesToDraw = planets[i].m_model.getNumTriangles();
  }

  // Use material properties from the loaded model
  m_Ka = planets[0].m_model.getKa();
  m_Kd = planets[0].m_model.getKd();
  m_Ks = planets[0].m_model.getKs();
  m_shininess = 13.0f;
}

void OpenGLWindow::paintGL() {
  update();

  glEnable(GL_CULL_FACE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  glUseProgram(m_program);

  GLint viewMatrixLoc{glGetUniformLocation(m_program, "viewMatrix")};
  GLint projMatrixLoc{glGetUniformLocation(m_program, "projMatrix")};
  GLint modelMatrixLoc{glGetUniformLocation(m_program, "modelMatrix")};
  GLint normalMatrixLoc{glGetUniformLocation(m_program, "normalMatrix")};
  GLint lightDirLoc{glGetUniformLocation(m_program, "lightDirWorldSpace")};
  GLint shininessLoc{glGetUniformLocation(m_program, "shininess")};
  GLint IaLoc{glGetUniformLocation(m_program, "Ia")};
  GLint IdLoc{glGetUniformLocation(m_program, "Id")};
  GLint IsLoc{glGetUniformLocation(m_program, "Is")};
  GLint KaLoc{glGetUniformLocation(m_program, "Ka")};
  GLint KdLoc{glGetUniformLocation(m_program, "Kd")};
  GLint KsLoc{glGetUniformLocation(m_program, "Ks")};
  GLint diffuseTexLoc{glGetUniformLocation(m_program, "diffuseTex")};
  GLint normalTexLoc{glGetUniformLocation(m_program, "normalTex")};
  GLint mappingModeLoc{glGetUniformLocation(m_program, "mappingMode")};

  glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_camera.m_viewMatrix[0][0]);
  glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_camera.m_projMatrix[0][0]);
  
  glUniform1i(diffuseTexLoc, 0);
  glUniform1i(normalTexLoc, 1);
  glUniform1i(mappingModeLoc, 3);

  glUniform4fv(lightDirLoc, 1, &m_lightDir.x);
  glUniform4fv(IaLoc, 1, &m_Ia.x);
  glUniform4fv(IdLoc, 1, &m_Id.x);
  glUniform4fv(IsLoc, 1, &m_Is.x);

  float mat[4] = {1.0f, 1.0f, 1.0f, 1.0f};
  glUniform1f(shininessLoc, 5000.0f);
  glUniform4fv(KaLoc, 1, mat);
  glUniform4fv(KdLoc, 1, mat);
  glUniform4fv(KsLoc, 1, mat);

  // Sun
  planets[9].m_modelMatrix = glm::mat4(1.0);
  planets[9].m_modelMatrix = glm::translate(planets[9].m_modelMatrix, glm::vec3(-3.5f, 0.0f, 0.0f));
  planets[9].m_modelMatrix = glm::rotate(planets[9].m_modelMatrix, glm::radians(0.005f * numberFramers), glm::vec3(0, 0, 1));
  planets[9].m_modelMatrix = glm::scale(planets[9].m_modelMatrix, glm::vec3(2.0f));
  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &planets[9].m_modelMatrix[0][0]);

  auto modelViewMatrix{glm::mat3(m_camera.m_viewMatrix * planets[9].m_modelMatrix)};
  glm::mat3 normalMatrix{glm::inverseTranspose(modelViewMatrix)};
  glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  planets[9].m_model.render(planets[9].m_trianglesToDraw);

  //------

  glUniform1f(shininessLoc, m_shininess);
  glUniform4fv(KaLoc, 1, &m_Ka.x);
  glUniform4fv(KdLoc, 1, &m_Kd.x);
  glUniform4fv(KsLoc, 1, &m_Ks.x);

  // Mercury
  planets[0].m_modelMatrix = glm::mat4(1.0);
  planets[0].m_modelMatrix = glm::translate(planets[0].m_modelMatrix, glm::vec3(-2.15f, 0.0f, 0.0f));
  planets[0].m_modelMatrix = glm::rotate(planets[0].m_modelMatrix, glm::radians(0.1f * numberFramers), glm::vec3(0, 1, 0));
  planets[0].m_modelMatrix = glm::scale(planets[0].m_modelMatrix, glm::vec3(0.2f));
  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &planets[0].m_modelMatrix[0][0]);

  modelViewMatrix = glm::mat3(m_camera.m_viewMatrix * planets[0].m_modelMatrix);
  normalMatrix = glm::inverseTranspose(modelViewMatrix);
  glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  planets[0].m_model.render(planets[0].m_trianglesToDraw);
  //------

  // Venus
  planets[1].m_modelMatrix = glm::mat4(1.0);
  planets[1].m_modelMatrix = glm::translate(planets[1].m_modelMatrix, glm::vec3(-1.75f, 0.0f, 0.0f));
  planets[1].m_modelMatrix = glm::rotate(planets[1].m_modelMatrix, glm::radians(0.03f * numberFramers), glm::vec3(0, 1, 0));
  planets[1].m_modelMatrix = glm::scale(planets[1].m_modelMatrix, glm::vec3(0.35f));
  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &planets[1].m_modelMatrix[0][0]);

  modelViewMatrix = glm::mat3(m_camera.m_viewMatrix * planets[1].m_modelMatrix);
  normalMatrix = glm::inverseTranspose(modelViewMatrix);
  glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  planets[1].m_model.render(planets[1].m_trianglesToDraw);
  //------

  // Earth
  planets[2].m_modelMatrix = glm::mat4(1.0);
  planets[2].m_modelMatrix = glm::translate(planets[2].m_modelMatrix, glm::vec3(-1.25f, 0.0f, 0.0f));
  planets[2].m_modelMatrix = glm::rotate(planets[2].m_modelMatrix, glm::radians(0.05f * numberFramers), glm::vec3(0, 1, 0));
  planets[2].m_modelMatrix = glm::scale(planets[2].m_modelMatrix, glm::vec3(0.4f));
  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &planets[2].m_modelMatrix[0][0]);

  modelViewMatrix = glm::mat3(m_camera.m_viewMatrix * planets[2].m_modelMatrix);
  normalMatrix = glm::inverseTranspose(modelViewMatrix);
  glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  planets[2].m_model.render(planets[2].m_trianglesToDraw);
  //------

  // Mars
  planets[3].m_modelMatrix = glm::mat4(1.0);
  planets[3].m_modelMatrix = glm::translate(planets[3].m_modelMatrix, glm::vec3(-0.75f, 0.0f, 0.0f));
  planets[3].m_modelMatrix = glm::rotate(planets[3].m_modelMatrix, glm::radians(90.0f), glm::vec3(1, 0, 0));
  planets[3].m_modelMatrix = glm::rotate(planets[3].m_modelMatrix, glm::radians(0.12f * numberFramers), glm::vec3(0, 0, -1));
  planets[3].m_modelMatrix = glm::scale(planets[3].m_modelMatrix, glm::vec3(0.35f));
  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &planets[3].m_modelMatrix[0][0]);

  modelViewMatrix = glm::mat3(m_camera.m_viewMatrix * planets[3].m_modelMatrix);
  normalMatrix = glm::inverseTranspose(modelViewMatrix);
  glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  planets[3].m_model.render(planets[3].m_trianglesToDraw);
  //------

  // Jupyter
  planets[4].m_modelMatrix = glm::mat4(1.0);
  planets[4].m_modelMatrix = glm::translate(planets[4].m_modelMatrix, glm::vec3(0.11f, 0.0f, 0.0f));
  planets[4].m_modelMatrix = glm::rotate(planets[4].m_modelMatrix, glm::radians(90.0f), glm::vec3(1, 0, 0));
  planets[4].m_modelMatrix = glm::rotate(planets[4].m_modelMatrix, glm::radians(0.07f * numberFramers), glm::vec3(0, 0, -1));
  planets[4].m_modelMatrix = glm::scale(planets[4].m_modelMatrix, glm::vec3(1.0f));
  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &planets[4].m_modelMatrix[0][0]);

  modelViewMatrix = glm::mat3(m_camera.m_viewMatrix * planets[4].m_modelMatrix);
  normalMatrix = glm::inverseTranspose(modelViewMatrix);
  glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  planets[4].m_model.render(planets[4].m_trianglesToDraw);
  //------

  // Saturn
  planets[5].m_modelMatrix = glm::mat4(1.0);
  planets[5].m_modelMatrix = glm::translate(planets[5].m_modelMatrix, glm::vec3(1.5f, 0.0f, 0.0f));
  planets[5].m_modelMatrix = glm::rotate(planets[5].m_modelMatrix, glm::radians(0.002f * numberFramers), glm::vec3(1, 0, 0));
  planets[5].m_modelMatrix = glm::scale(planets[5].m_modelMatrix, glm::vec3(1.1f));
  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &planets[5].m_modelMatrix[0][0]);

  modelViewMatrix = glm::mat3(m_camera.m_viewMatrix * planets[5].m_modelMatrix);
  normalMatrix = glm::inverseTranspose(modelViewMatrix);
  glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  planets[5].m_model.render(planets[5].m_trianglesToDraw);
  //------

  // Uranus
  planets[6].m_modelMatrix = glm::mat4(1.0);
  planets[6].m_modelMatrix = glm::translate(planets[6].m_modelMatrix, glm::vec3(2.5f, 0.0f, 0.0f));
  planets[6].m_modelMatrix = glm::rotate(planets[6].m_modelMatrix, glm::radians(180.0f), glm::vec3(0, 1, 0));
  planets[6].m_modelMatrix = glm::rotate(planets[6].m_modelMatrix, glm::radians(0.004f * numberFramers), glm::vec3(1, 0, 0));
  planets[6].m_modelMatrix = glm::scale(planets[6].m_modelMatrix, glm::vec3(0.7f));
  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &planets[6].m_modelMatrix[0][0]);

  modelViewMatrix = glm::mat3(m_camera.m_viewMatrix * planets[6].m_modelMatrix);
  normalMatrix = glm::inverseTranspose(modelViewMatrix);
  glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  planets[6].m_model.render(planets[6].m_trianglesToDraw);
  //------

  // Neptune
  planets[7].m_modelMatrix = glm::mat4(1.0);
  planets[7].m_modelMatrix = glm::translate(planets[7].m_modelMatrix, glm::vec3(3.2f, 0.0f, 0.0f));
  planets[7].m_modelMatrix = glm::rotate(planets[7].m_modelMatrix, glm::radians(0.075f * numberFramers), glm::vec3(0, 1, 0));
  planets[7].m_modelMatrix = glm::scale(planets[7].m_modelMatrix, glm::vec3(0.45f));
  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &planets[7].m_modelMatrix[0][0]);

  modelViewMatrix = glm::mat3(m_camera.m_viewMatrix * planets[7].m_modelMatrix);
  normalMatrix = glm::inverseTranspose(modelViewMatrix);
  glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  planets[7].m_model.render(planets[7].m_trianglesToDraw);
  //------
  
  // Pluto
  planets[8].m_modelMatrix = glm::mat4(1.0);
  planets[8].m_modelMatrix = glm::translate(planets[8].m_modelMatrix, glm::vec3(3.7f, 0.0f, 0.0f));
  planets[8].m_modelMatrix = glm::rotate(planets[8].m_modelMatrix, glm::radians(0.4f * numberFramers), glm::vec3(0, 1, 0));
  planets[8].m_modelMatrix = glm::scale(planets[8].m_modelMatrix, glm::vec3(0.15f));
  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &planets[8].m_modelMatrix[0][0]);

  modelViewMatrix = glm::mat3(m_camera.m_viewMatrix * planets[8].m_modelMatrix);
  normalMatrix = glm::inverseTranspose(modelViewMatrix);
  glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  planets[8].m_model.render(planets[8].m_trianglesToDraw);
  //------

  numberFramers++;
  glUseProgram(0);
}

void OpenGLWindow::paintUI() {
  /*
  {
    // Header window
    auto windowWidth{m_viewportWidth * 1.0f};
    auto windowHeight{80};
    ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));
    ImGui::SetNextWindowPos(ImVec2((m_viewportWidth - windowWidth) / 2, 0));
    auto flags{ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize |
               ImGuiWindowFlags_NoCollapse};

      std::string text;
      text = fmt::format("{}:{}:{}\n{}:{}:{}\n{}:{}:{}\n{}", 
      m_camera.m_eye[0], m_camera.m_eye[1], m_camera.m_eye[2],
      m_camera.m_at[0], m_camera.m_at[1], m_camera.m_at[2],
      m_camera.m_up[0], m_camera.m_up[1], m_camera.m_up[2],
      numberFramers);

      ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize(text.c_str()).x) /2);
      ImGui::Text("%s", text.c_str());

    }
    */

    {
      auto aspect{static_cast<float>(m_viewportWidth) / static_cast<float>(m_viewportHeight)};
      m_projMatrix = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 5.0f);
    }
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;
  m_camera.computeProjectionMatrix(width, height);
}

void OpenGLWindow::terminateGL() {
  glDeleteProgram(m_program);
}

void OpenGLWindow::update() {
  float deltaTime{static_cast<float>(getDeltaTime())};
  m_camera.dolly(m_dollySpeed * deltaTime);
  m_camera.truck(m_truckSpeed * deltaTime);
  m_camera.pan(m_panSpeed * deltaTime);
  m_camera.lift(m_liftSpeed * deltaTime);
}