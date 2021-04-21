## Sistema Solar (Projeto 3) - Computação Gráfica 2021.1

Autora: **Eduarda Meirinhos - RA 11076516**

**Link do repositório do código-fonte:** https://github.com/EduardaMeirinhos/abcg/tree/main/examples/solarsystem

**Link para uma página Web com a aplicação rodando em WebAssembly:** https://eduardameirinhos.github.io/play-solar-system/

Este repositório contém o código-fonte da implementação de uma simulação interativa do Sistema Solar, desenvolvido como Projeto 3 para a disciplina de Computação Gráfica, em 2021.1, ministrada pelo Prof. Harlen Batagelo.

O projeto foi desenvolvido utilizando a biblioteca **abcg** e **modelos gratuitos 3D do site [Free3D](https://free3d.com/)**. O código está baseado nas implementações dos projetos Viewer5 (carregador e visualizador de modelos) e LookAt (câmera livre), disponibilizados na página da disciplina. Foram explorados os recursos de iluminação, texturização e movimentação de câmera para interatividade.

Para tanto, foram carregados dez modelos **.obj** e suas texturas em formato **.jpg** e **.png**. Os shaders utilizados para implementação da iluminação foram o **texture.vert** e **texture.frag**. Para a câmera, foram utilizados **lookat.vert** e **lookat.frag**. O tipo de mapping utilizado foi o **from mash** sempre embasado em **diffuse map**. Os modelos representam os seguintes corpos celestes:

- [Sol](https://free3d.com/3d-model/sun-43982.html)
- [Mercúrio](https://free3d.com/3d-model/mercury-23007.html)
- [Vênus](https://free3d.com/3d-model/venus-98714.html)
- [Terra](https://free3d.com/3d-model/photorealistic-earth-98256.html)
- [Marte](https://free3d.com/3d-model/mars-photorealistic-2k-671043.html)
- [Júpiter](https://free3d.com/3d-model/jupiter-v1--853820.html)
- [Saturno](https://free3d.com/3d-model/saturn-v1--741827.html)
- [Urano](https://free3d.com/3d-model/uranus-v2--767518.html)
- [Netuno](https://free3d.com/3d-model/neptune-82847.html)
- [Plutão](https://free3d.com/3d-model/pluto-v1--424613.html)

Cada corpo possui seu próprio tempo de rotação, eixo de rotação e tamanho (escala). Além disso, foram implementados dois conjuntos de propriedades de luz e material diferentes: o primeiro para definir o brilho do Sol (como esfera brilhante) e o segundo para representar a incidência da luz nos planetas a partir da direção em que se encontra o Sol.

A interatividade proposta está relacionada à movimentação da câmera, que pode ser controlada livremente (como descrito abaixo), conservando as direções definidas da luz solar. Foi implementado um movimento adicional (movimento vertical), que antes não existia no projeto LookAt.

**Controles de Movimentação da Câmera**

-  **W**: Move a câmera para frente
-  **A**: Move a câmera circularmente para a esquerda
-  **S**: Move a câmera para trás
-  **D**: Move a câmera circularmente para a direita
-  **Q**: Move a câmera lateralmente para a esquerda
-  **E**: Move a câmera lateralmente para a direita
-  **R**: Move a câmera verticalmente para cima
-  **F**: Move a câmera verticalmente para baixo