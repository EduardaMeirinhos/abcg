#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <string_view>

#include "abcg.hpp"
#include "model.hpp"
#include "camera.hpp"
#include <string>

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void handleEvent(SDL_Event& ev) override;
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:
  struct Planet
  {
    Model m_model;
    int m_trianglesToDraw{};
    glm::mat4 m_modelMatrix{1.0f};
  };

  int m_viewportWidth{};
  int m_viewportHeight{};

  Camera m_camera;
  float m_zoom{};
  float m_dollySpeed{0.0f};
  float m_truckSpeed{0.0f};
  float m_panSpeed{0.0f};
  float m_liftSpeed{0.0f};

  glm::vec4 m_lightDir{0.5f, 0.0f, 0.0f, 0.0f};

  Planet planets[10];
  std::string filenames[10][2] = {
    "mercury.obj", "mercury_map.jpg",
    "venus.obj", "venus_map.jpg",
    "earth.obj", "earth_map.png",
    "mars.obj", "mars_map.jpg",
    "jupyter.obj", "jupyter_map.jpg",
    "saturn.obj", "saturn_map.jpg",
    "uranus.obj", "uranus_map.jpg",
    "neptune.obj", "neptune_map.jpg",
    "pluto.obj", "pluto_map.jpg",
    "sun.obj", "sun_map.jpg"
  };

  glm::mat4 m_viewMatrix{1.0f};
  glm::mat4 m_projMatrix{1.0f};
  glm::vec4 m_Ia{1.0f};
  glm::vec4 m_Id{1.0f};
  glm::vec4 m_Is{1.0f, 0.861f, 0.591f, 1.0f};
  //glm::vec4 m_Is{1.0f};
  glm::vec4 m_Ka{};
  glm::vec4 m_Kd{};
  glm::vec4 m_Ks{};
  float m_shininess{};

  // Shaders
  GLuint m_program;
  int m_currentProgramIndex{};

  unsigned long long int numberFramers{1};

  void loadModel();
  void update();
};

#endif