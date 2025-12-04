#include "decorations.h"

// Função auxiliar para carregar decorações padrão (usada nos levels 1 e 2)
static void loadStandardDecorations(std::vector<DecoObject>& hexagons_3D,
                                    std::vector<DecoObject>& flowers_3D,
                                    std::vector<DecoObject>& plants_3D,
                                    std::vector<DecoObject>& rocks_3D,
                                    std::vector<DecoObject>& pines_3D,
                                    std::vector<DecoObject>& trees_3D) {
    // Hexágonos (Z positivo)
    hexagons_3D.push_back({-180.0f, -100.0f, 50.0f, 24.0f, 0.0f});
    hexagons_3D.push_back({250.0f, -100.0f, 120.0f, 20.0f, 45.0f});
    hexagons_3D.push_back({-320.0f, -100.0f, 200.0f, 26.0f, 90.0f});
    hexagons_3D.push_back({150.0f, -100.0f, 280.0f, 18.0f, 135.0f});
    hexagons_3D.push_back({-250.0f, -100.0f, 360.0f, 22.0f, 180.0f});
    hexagons_3D.push_back({350.0f, -100.0f, 440.0f, 20.0f, 225.0f});
    hexagons_3D.push_back({-100.0f, -100.0f, 520.0f, 23.0f, 270.0f});
    hexagons_3D.push_back({280.0f, -100.0f, 600.0f, 21.0f, 315.0f});
    hexagons_3D.push_back({-380.0f, -100.0f, 680.0f, 25.0f, 360.0f});
    hexagons_3D.push_back({80.0f, -100.0f, 760.0f, 22.0f, 405.0f});
    hexagons_3D.push_back({-18.0f, -100.0f, 50.0f, 24.0f, 0.0f});
    hexagons_3D.push_back({10.0f, -100.0f, 120.0f, 20.0f, 45.0f});
    hexagons_3D.push_back({-40.0f, -100.0f, 200.0f, 26.0f, 90.0f});
    hexagons_3D.push_back({8.0f, -100.0f, 280.0f, 18.0f, 135.0f});
    hexagons_3D.push_back({-20.0f, -100.0f, 360.0f, 22.0f, 180.0f});
    hexagons_3D.push_back({50.0f, -100.0f, 440.0f, 20.0f, 225.0f});
    hexagons_3D.push_back({-80.0f, -100.0f, 100.0f, 23.0f, 60.0f});
    hexagons_3D.push_back({80.0f, -100.0f, 180.0f, 21.0f, 120.0f});
    hexagons_3D.push_back({-30.0f, -100.0f, 260.0f, 25.0f, 180.0f});
    hexagons_3D.push_back({10.0f, -100.0f, 330.0f, 22.0f, 240.0f});
    hexagons_3D.push_back({5.0f, -100.0f, 500.0f, 22.4f, 300.0f});
    hexagons_3D.push_back({-15.0f, -100.0f, 580.0f, 21.6f, 330.0f});
    hexagons_3D.push_back({25.0f, -100.0f, 660.0f, 23.0f, 360.0f});
    hexagons_3D.push_back({-5.0f, -100.0f, 730.0f, 20.6f, 390.0f});
    // Hexágonos (Z negativo)
    hexagons_3D.push_back({-190.0f, -100.0f, -60.0f, 24.0f, 450.0f});
    hexagons_3D.push_back({260.0f, -100.0f, -140.0f, 20.0f, 495.0f});
    hexagons_3D.push_back({-310.0f, -100.0f, -220.0f, 26.0f, 540.0f});
    hexagons_3D.push_back({160.0f, -100.0f, -300.0f, 18.0f, 585.0f});
    hexagons_3D.push_back({-240.0f, -100.0f, -380.0f, 22.0f, 630.0f});
    hexagons_3D.push_back({360.0f, -100.0f, -460.0f, 20.0f, 675.0f});
    hexagons_3D.push_back({-110.0f, -100.0f, -540.0f, 23.0f, 720.0f});
    hexagons_3D.push_back({290.0f, -100.0f, -620.0f, 21.0f, 765.0f});
    hexagons_3D.push_back({-370.0f, -100.0f, -700.0f, 25.0f, 810.0f});
    hexagons_3D.push_back({90.0f, -100.0f, -780.0f, 22.0f, 855.0f});
    hexagons_3D.push_back({-8.0f, -100.0f, -50.0f, 24.0f, 900.0f});
    hexagons_3D.push_back({15.0f, -100.0f, -130.0f, 20.0f, 945.0f});
    hexagons_3D.push_back({-25.0f, -100.0f, -210.0f, 26.0f, 990.0f});
    hexagons_3D.push_back({12.0f, -100.0f, -290.0f, 18.0f, 1035.0f});
    hexagons_3D.push_back({-18.0f, -100.0f, -370.0f, 22.0f, 1080.0f});
    hexagons_3D.push_back({22.0f, -100.0f, -450.0f, 20.0f, 1125.0f});
    hexagons_3D.push_back({-12.0f, -100.0f, -530.0f, 23.0f, 1170.0f});
    hexagons_3D.push_back({8.0f, -100.0f, -610.0f, 21.0f, 1215.0f});
    hexagons_3D.push_back({-20.0f, -100.0f, -690.0f, 25.0f, 1260.0f});
    hexagons_3D.push_back({5.0f, -100.0f, -770.0f, 22.0f, 1305.0f});

    // Flores (agrupamentos Z positivo)
    flowers_3D.push_back({-220.0f, -100.0f, 70.0f, 16.0f, 0.0f});
    flowers_3D.push_back({-200.0f, -100.0f, 72.0f, 18.0f, 30.0f});
    flowers_3D.push_back({10.0f, -100.0f, 160.0f, 17.0f, 60.0f});
    flowers_3D.push_back({330.0f, -100.0f, 162.0f, 16.0f, 90.0f});
    flowers_3D.push_back({-170.0f, -100.0f, 250.0f, 18.0f, 120.0f});
    flowers_3D.push_back({-15.0f, -100.0f, 252.0f, 17.0f, 150.0f});
    flowers_3D.push_back({190.0f, -100.0f, 340.0f, 16.0f, 180.0f});
    flowers_3D.push_back({60.0f, -100.0f, 342.0f, 18.0f, 210.0f});
    flowers_3D.push_back({5.0f, -100.0f, 500.0f, 16.4f, 1100.0f});
    flowers_3D.push_back({8.0f, -100.0f, 502.0f, 17.4f, 1130.0f});
    flowers_3D.push_back({-12.0f, -100.0f, 630.0f, 17.0f, 1160.0f});
    flowers_3D.push_back({-10.0f, -100.0f, 632.0f, 16.6f, 1190.0f});
    flowers_3D.push_back({-290.0f, -100.0f, 420.0f, 17.0f, 240.0f});
    flowers_3D.push_back({-27.0f, -100.0f, 422.0f, 16.0f, 270.0f});
    flowers_3D.push_back({120.0f, -100.0f, 510.0f, 16.4f, 300.0f});
    flowers_3D.push_back({30.0f, -100.0f, 512.0f, 17.4f, 330.0f});
    flowers_3D.push_back({-340.0f, -100.0f, 590.0f, 16.6f, 360.0f});
    flowers_3D.push_back({-32.0f, -100.0f, 592.0f, 17.2f, 390.0f});
    flowers_3D.push_back({240.0f, -100.0f, 670.0f, 16.8f, 420.0f});
    flowers_3D.push_back({20.0f, -100.0f, 672.0f, 16.2f, 450.0f});
    flowers_3D.push_back({-130.0f, -100.0f, 750.0f, 17.0f, 480.0f});
    flowers_3D.push_back({-10.0f, -100.0f, 752.0f, 16.4f, 510.0f});
    // Flores (agrupamentos Z negativo)
    flowers_3D.push_back({-230.0f, -100.0f, -80.0f, 16.0f, 540.0f});
    flowers_3D.push_back({-210.0f, -100.0f, -82.0f, 18.0f, 570.0f});
    flowers_3D.push_back({20.0f, -100.0f, -170.0f, 17.0f, 600.0f});
    flowers_3D.push_back({340.0f, -100.0f, -172.0f, 16.0f, 630.0f});
    flowers_3D.push_back({-180.0f, -100.0f, -260.0f, 18.0f, 660.0f});
    flowers_3D.push_back({-25.0f, -100.0f, -262.0f, 17.0f, 690.0f});
    flowers_3D.push_back({200.0f, -100.0f, -350.0f, 16.0f, 720.0f});
    flowers_3D.push_back({70.0f, -100.0f, -352.0f, 18.0f, 750.0f});
    flowers_3D.push_back({12.0f, -100.0f, -180.0f, 17.0f, 1220.0f});
    flowers_3D.push_back({15.0f, -100.0f, -182.0f, 16.0f, 1250.0f});
    flowers_3D.push_back({-8.0f, -100.0f, -270.0f, 18.0f, 1280.0f});
    flowers_3D.push_back({-5.0f, -100.0f, -272.0f, 17.0f, 1310.0f});
    flowers_3D.push_back({18.0f, -100.0f, -360.0f, 16.0f, 1340.0f});
    flowers_3D.push_back({20.0f, -100.0f, -362.0f, 18.0f, 1370.0f});
    flowers_3D.push_back({-10.0f, -100.0f, -510.0f, 16.4f, 1400.0f});
    flowers_3D.push_back({-8.0f, -100.0f, -512.0f, 17.4f, 1430.0f});
    flowers_3D.push_back({8.0f, -100.0f, -640.0f, 17.0f, 1460.0f});
    flowers_3D.push_back({10.0f, -100.0f, -642.0f, 16.6f, 1490.0f});
    flowers_3D.push_back({-300.0f, -100.0f, -430.0f, 17.0f, 780.0f});
    flowers_3D.push_back({-35.0f, -100.0f, -432.0f, 16.0f, 810.0f});
    flowers_3D.push_back({130.0f, -100.0f, -520.0f, 16.4f, 840.0f});
    flowers_3D.push_back({40.0f, -100.0f, -522.0f, 17.4f, 870.0f});
    flowers_3D.push_back({-350.0f, -100.0f, -600.0f, 16.6f, 900.0f});
    flowers_3D.push_back({-40.0f, -100.0f, -602.0f, 17.2f, 930.0f});
    flowers_3D.push_back({250.0f, -100.0f, -680.0f, 16.8f, 960.0f});
    flowers_3D.push_back({30.0f, -100.0f, -682.0f, 16.2f, 990.0f});
    flowers_3D.push_back({-140.0f, -100.0f, -760.0f, 17.0f, 1020.0f});
    flowers_3D.push_back({-5.0f, -100.0f, -762.0f, 16.4f, 1050.0f});

    // Plantas (Z positivo)
    plants_3D.push_back({-120.0f, -100.0f, 90.0f, 22.0f, 15.0f});
    plants_3D.push_back({260.0f, -100.0f, 180.0f, 20.0f, 45.0f});
    plants_3D.push_back({-30.0f, -100.0f, 270.0f, 24.0f, 75.0f});
    plants_3D.push_back({110.0f, -100.0f, 350.0f, 19.0f, 105.0f});
    plants_3D.push_back({-280.0f, -100.0f, 430.0f, 22.0f, 135.0f});
    plants_3D.push_back({320.0f, -100.0f, 520.0f, 21.0f, 165.0f});
    plants_3D.push_back({-160.0f, -100.0f, 600.0f, 22.4f, 195.0f});
    plants_3D.push_back({200.0f, -100.0f, 680.0f, 21.6f, 225.0f});
    plants_3D.push_back({-390.0f, -100.0f, 760.0f, 19.6f, 255.0f});
    plants_3D.push_back({60.0f, -100.0f, 140.0f, 23.0f, 285.0f});
    plants_3D.push_back({-240.0f, -100.0f, 220.0f, 20.6f, 315.0f});
    plants_3D.push_back({60.0f, -100.0f, 10.0f, 23.0f, 285.0f});
    plants_3D.push_back({15.0f, -100.0f, 550.0f, 20.4f, 1520.0f});
    plants_3D.push_back({-20.0f, -100.0f, 700.0f, 22.0f, 1550.0f});
    // Plantas (Z negativo)
    plants_3D.push_back({-130.0f, -100.0f, -100.0f, 22.0f, 345.0f});
    plants_3D.push_back({270.0f, -100.0f, -190.0f, 20.0f, 375.0f});
    plants_3D.push_back({-40.0f, -100.0f, -280.0f, 24.0f, 405.0f});
    plants_3D.push_back({120.0f, -100.0f, -360.0f, 19.0f, 435.0f});
    plants_3D.push_back({-290.0f, -100.0f, -440.0f, 22.0f, 465.0f});
    plants_3D.push_back({330.0f, -100.0f, -530.0f, 21.0f, 495.0f});
    plants_3D.push_back({-170.0f, -100.0f, -610.0f, 22.4f, 525.0f});
    plants_3D.push_back({210.0f, -100.0f, -690.0f, 21.6f, 555.0f});
    plants_3D.push_back({-380.0f, -100.0f, -770.0f, 19.6f, 585.0f});
    plants_3D.push_back({70.0f, -100.0f, -150.0f, 23.0f, 615.0f});
    plants_3D.push_back({10.0f, -100.0f, -250.0f, 21.6f, 1580.0f});
    plants_3D.push_back({-15.0f, -100.0f, -400.0f, 22.4f, 1610.0f});
    plants_3D.push_back({18.0f, -100.0f, -550.0f, 21.0f, 1640.0f});
    plants_3D.push_back({-8.0f, -100.0f, -720.0f, 22.0f, 1670.0f});

    // Rochas (Z positivo)
    rocks_3D.push_back({-240.0f, -100.0f, 110.0f, 26.0f, 0.0f});
    rocks_3D.push_back({300.0f, -100.0f, 190.0f, 22.0f, 60.0f});
    rocks_3D.push_back({-220.0f, -100.0f, 290.0f, 28.0f, 120.0f});
    rocks_3D.push_back({340.0f, -100.0f, 370.0f, 24.0f, 180.0f});
    rocks_3D.push_back({-210.0f, -100.0f, 450.0f, 26.0f, 240.0f});
    rocks_3D.push_back({180.0f, -100.0f, 530.0f, 25.0f, 300.0f});
    rocks_3D.push_back({-370.0f, -100.0f, 610.0f, 27.0f, 360.0f});
    rocks_3D.push_back({90.0f, -100.0f, 690.0f, 23.0f, 420.0f});
    rocks_3D.push_back({-150.0f, -100.0f, 770.0f, 29.0f, 480.0f});
    rocks_3D.push_back({270.0f, -100.0f, 50.0f, 25.6f, 540.0f});
    rocks_3D.push_back({-10.0f, -100.0f, 580.0f, 26.4f, 1700.0f});
    rocks_3D.push_back({20.0f, -100.0f, 710.0f, 25.0f, 1730.0f});
    // Rochas (Z negativo)
    rocks_3D.push_back({-250.0f, -100.0f, -120.0f, 26.0f, 600.0f});
    rocks_3D.push_back({310.0f, -100.0f, -200.0f, 22.0f, 660.0f});
    rocks_3D.push_back({-230.0f, -100.0f, -300.0f, 28.0f, 720.0f});
    rocks_3D.push_back({350.0f, -100.0f, -380.0f, 24.0f, 780.0f});
    rocks_3D.push_back({-220.0f, -100.0f, -460.0f, 26.0f, 840.0f});
    rocks_3D.push_back({190.0f, -100.0f, -540.0f, 25.0f, 900.0f});
    rocks_3D.push_back({-360.0f, -100.0f, -620.0f, 27.0f, 960.0f});
    rocks_3D.push_back({100.0f, -100.0f, -700.0f, 23.0f, 1020.0f});
    rocks_3D.push_back({-160.0f, -100.0f, -780.0f, 29.0f, 1080.0f});
    rocks_3D.push_back({280.0f, -100.0f, -60.0f, 25.6f, 1140.0f});
    rocks_3D.push_back({12.0f, -100.0f, -180.0f, 26.0f, 1760.0f});
    rocks_3D.push_back({-18.0f, -100.0f, -330.0f, 28.0f, 1790.0f});
    rocks_3D.push_back({8.0f, -100.0f, -480.0f, 27.0f, 1820.0f});
    rocks_3D.push_back({-12.0f, -100.0f, -650.0f, 24.0f, 1850.0f});
    rocks_3D.push_back({15.0f, -100.0f, -740.0f, 27.6f, 1880.0f});

    // Pinheiros (Z positivo)
    pines_3D.push_back({-100.0f, -100.0f, 60.0f, 30.0f, 0.0f});
    pines_3D.push_back({230.0f, -100.0f, 140.0f, 28.0f, 45.0f});
    pines_3D.push_back({-330.0f, -100.0f, 230.0f, 32.0f, 90.0f});
    pines_3D.push_back({170.0f, -100.0f, 320.0f, 30.0f, 135.0f});
    pines_3D.push_back({-260.0f, -100.0f, 400.0f, 28.0f, 180.0f});
    pines_3D.push_back({350.0f, -100.0f, 480.0f, 30.0f, 225.0f});
    pines_3D.push_back({-140.0f, -100.0f, 560.0f, 31.0f, 270.0f});
    pines_3D.push_back({290.0f, -100.0f, 640.0f, 29.0f, 315.0f});
    pines_3D.push_back({-380.0f, -100.0f, 720.0f, 33.0f, 360.0f});
    pines_3D.push_back({50.0f, -100.0f, 100.0f, 30.4f, 405.0f});
    pines_3D.push_back({-190.0f, -100.0f, 180.0f, 29.6f, 450.0f});
    pines_3D.push_back({10.0f, -100.0f, 610.0f, 30.6f, 1910.0f});
    pines_3D.push_back({-15.0f, -100.0f, 780.0f, 29.4f, 1940.0f});
    // Pinheiros (Z negativo)
    pines_3D.push_back({-110.0f, -100.0f, -70.0f, 30.0f, 495.0f});
    pines_3D.push_back({240.0f, -100.0f, -150.0f, 28.0f, 540.0f});
    pines_3D.push_back({-340.0f, -100.0f, -240.0f, 32.0f, 585.0f});
    pines_3D.push_back({180.0f, -100.0f, -330.0f, 30.0f, 630.0f});
    pines_3D.push_back({-270.0f, -100.0f, -410.0f, 28.0f, 675.0f});
    pines_3D.push_back({360.0f, -100.0f, -490.0f, 30.0f, 720.0f});
    pines_3D.push_back({-150.0f, -100.0f, -570.0f, 31.0f, 765.0f});
    pines_3D.push_back({300.0f, -100.0f, -650.0f, 29.0f, 810.0f});
    pines_3D.push_back({-370.0f, -100.0f, -730.0f, 33.0f, 855.0f});
    pines_3D.push_back({60.0f, -100.0f, -110.0f, 30.4f, 900.0f});
    pines_3D.push_back({-10.0f, -100.0f, -200.0f, 30.0f, 1970.0f});
    pines_3D.push_back({18.0f, -100.0f, -350.0f, 32.0f, 2000.0f});
    pines_3D.push_back({-12.0f, -100.0f, -500.0f, 29.0f, 2030.0f});
    pines_3D.push_back({8.0f, -100.0f, -680.0f, 31.6f, 2060.0f});

    // Árvores normais (Z positivo)
    trees_3D.push_back({-80.0f, -100.0f, 80.0f, 36.0f, 30.0f});
    trees_3D.push_back({220.0f, -100.0f, 170.0f, 34.0f, 60.0f});
    trees_3D.push_back({-350.0f, -100.0f, 260.0f, 38.0f, 90.0f});
    trees_3D.push_back({130.0f, -100.0f, 350.0f, 36.0f, 120.0f});
    trees_3D.push_back({-270.0f, -100.0f, 440.0f, 34.0f, 150.0f});
    trees_3D.push_back({310.0f, -100.0f, 530.0f, 35.0f, 180.0f});
    trees_3D.push_back({-200.0f, -100.0f, 620.0f, 37.0f, 210.0f});
    trees_3D.push_back({160.0f, -100.0f, 700.0f, 35.6f, 240.0f});
    trees_3D.push_back({-390.0f, -100.0f, 40.0f, 38.4f, 270.0f});
    trees_3D.push_back({280.0f, -100.0f, 120.0f, 36.6f, 300.0f});
    trees_3D.push_back({-110.0f, -100.0f, 200.0f, 35.2f, 330.0f});
    trees_3D.push_back({12.0f, -100.0f, 590.0f, 34.4f, 2090.0f});
    trees_3D.push_back({-18.0f, -100.0f, 740.0f, 37.0f, 2120.0f});
    // Árvores normais (Z negativo)
    trees_3D.push_back({-90.0f, -100.0f, -90.0f, 36.0f, 360.0f});
    trees_3D.push_back({230.0f, -100.0f, -180.0f, 34.0f, 390.0f});
    trees_3D.push_back({-360.0f, -100.0f, -270.0f, 38.0f, 420.0f});
    trees_3D.push_back({140.0f, -100.0f, -360.0f, 36.0f, 450.0f});
    trees_3D.push_back({-280.0f, -100.0f, -450.0f, 34.0f, 480.0f});
    trees_3D.push_back({320.0f, -100.0f, -540.0f, 35.0f, 510.0f});
    trees_3D.push_back({-210.0f, -100.0f, -630.0f, 37.0f, 540.0f});
    trees_3D.push_back({170.0f, -100.0f, -710.0f, 35.6f, 570.0f});
    trees_3D.push_back({-380.0f, -100.0f, -50.0f, 38.4f, 600.0f});
    trees_3D.push_back({290.0f, -100.0f, -130.0f, 36.6f, 630.0f});
    trees_3D.push_back({-8.0f, -100.0f, -220.0f, 35.0f, 2150.0f});
    trees_3D.push_back({15.0f, -100.0f, -390.0f, 36.0f, 2180.0f});
    trees_3D.push_back({-12.0f, -100.0f, -560.0f, 35.6f, 2210.0f});
    trees_3D.push_back({10.0f, -100.0f, -720.0f, 36.4f, 2240.0f});
}

void loadLevel1Decorations(std::vector<DecoObject>& hexagons_3D,
                          std::vector<DecoObject>& flowers_3D,
                          std::vector<DecoObject>& plants_3D,
                          std::vector<DecoObject>& rocks_3D,
                          std::vector<DecoObject>& pines_3D,
                          std::vector<DecoObject>& trees_3D) {
    loadStandardDecorations(hexagons_3D, flowers_3D, plants_3D, rocks_3D, pines_3D, trees_3D);
}

void loadLevel2Decorations(std::vector<DecoObject>& hexagons_3D,
                          std::vector<DecoObject>& flowers_3D,
                          std::vector<DecoObject>& plants_3D,
                          std::vector<DecoObject>& rocks_3D,
                          std::vector<DecoObject>& pines_3D,
                          std::vector<DecoObject>& trees_3D) {
    loadStandardDecorations(hexagons_3D, flowers_3D, plants_3D, rocks_3D, pines_3D, trees_3D);
}

void loadLevel3Decorations(std::vector<DecoObject>& hexagons_3D,
                          std::vector<DecoObject>& flowers_3D,
                          std::vector<DecoObject>& plants_3D,
                          std::vector<DecoObject>& rocks_3D,
                          std::vector<DecoObject>& pines_3D,
                          std::vector<DecoObject>& trees_3D) {
    // Level 3: Decorações para deserto/vulcão - será implementado posteriormente
}
