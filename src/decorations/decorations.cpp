#include "decorations.h"

void loadLevel1Decorations(std::vector<DecoObject>& hexagons_3D,
                          std::vector<DecoObject>& flowers_3D,
                          std::vector<DecoObject>& plants_3D,
                          std::vector<DecoObject>& rocks_3D,
                          std::vector<DecoObject>& pines_3D,
                          std::vector<DecoObject>& trees_3D,
                          std::vector<DecoObject>& decoration1_3D,
                          std::vector<DecoObject>& decoration2_3D,
                          std::vector<DecoObject>& decoration3_3D) {
    // Level 1: Decorações de floresta

    // Hexágonos
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

    // Flores
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

    // Plantas
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

    // Rochas
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

    // Pinheiros
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

    // Árvores normais 
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
    trees_3D.push_back({-500.0f, -100.0f, 60.0f, 37.0f, 2245.0f});
    trees_3D.push_back({520.0f, -100.0f, 80.0f, 35.5f, 2250.0f});
    trees_3D.push_back({-600.0f, -100.0f, 150.0f, 36.0f, 2255.0f});
    trees_3D.push_back({650.0f, -100.0f, 230.0f, 38.0f, 2260.0f});
    trees_3D.push_back({-480.0f, -100.0f, 770.0f, 37.5f, 2265.0f});
    trees_3D.push_back({570.0f, -100.0f, 800.0f, 36.5f, 2270.0f});
    trees_3D.push_back({-35.0f, -100.0f, 20.0f, 34.0f, 2275.0f});
    trees_3D.push_back({45.0f, -100.0f, 30.0f, 35.0f, 2280.0f});
    trees_3D.push_back({-520.0f, -100.0f, -70.0f, 37.0f, 2285.0f});
    trees_3D.push_back({540.0f, -100.0f, -90.0f, 36.0f, 2290.0f});
    trees_3D.push_back({-620.0f, -100.0f, -160.0f, 38.0f, 2295.0f});
    trees_3D.push_back({670.0f, -100.0f, -240.0f, 35.5f, 2300.0f});
    trees_3D.push_back({-500.0f, -100.0f, -780.0f, 36.5f, 2305.0f});
    trees_3D.push_back({590.0f, -100.0f, -810.0f, 37.5f, 2310.0f});
    trees_3D.push_back({-40.0f, -100.0f, -22.0f, 34.5f, 2315.0f});
    trees_3D.push_back({50.0f, -100.0f, -32.0f, 35.5f, 2320.0f});
    pines_3D.push_back({-450.0f, -100.0f, 35.0f, 31.0f, 2325.0f});
    pines_3D.push_back({480.0f, -100.0f, 45.0f, 29.5f, 2330.0f});
    pines_3D.push_back({-550.0f, -100.0f, 120.0f, 30.0f, 2335.0f});
    pines_3D.push_back({600.0f, -100.0f, 200.0f, 31.5f, 2340.0f});
    pines_3D.push_back({-420.0f, -100.0f, 750.0f, 29.0f, 2345.0f});
    pines_3D.push_back({530.0f, -100.0f, 780.0f, 30.5f, 2350.0f});
    pines_3D.push_back({-25.0f, -100.0f, 15.0f, 28.0f, 2355.0f});
    pines_3D.push_back({35.0f, -100.0f, 25.0f, 29.0f, 2360.0f});
    pines_3D.push_back({-470.0f, -100.0f, -40.0f, 30.5f, 2365.0f});
    pines_3D.push_back({510.0f, -100.0f, -50.0f, 29.0f, 2370.0f});
    pines_3D.push_back({-580.0f, -100.0f, -130.0f, 31.0f, 2375.0f});
    pines_3D.push_back({620.0f, -100.0f, -210.0f, 30.0f, 2380.0f});
    pines_3D.push_back({-440.0f, -100.0f, -760.0f, 30.5f, 2385.0f});
    pines_3D.push_back({560.0f, -100.0f, -790.0f, 29.5f, 2390.0f});
    pines_3D.push_back({-30.0f, -100.0f, -18.0f, 28.5f, 2395.0f});
    pines_3D.push_back({40.0f, -100.0f, -28.0f, 29.5f, 2400.0f});

    // Decoração 1
    decoration1_3D.push_back({-160.0f, -100.0f, 100.0f, 28.0f, 0.0f});
    decoration1_3D.push_back({220.0f, -100.0f, 200.0f, 30.0f, 45.0f});
    decoration1_3D.push_back({-280.0f, -100.0f, 300.0f, 29.0f, 90.0f});
    decoration1_3D.push_back({160.0f, -100.0f, 400.0f, 27.0f, 135.0f});
    decoration1_3D.push_back({-320.0f, -100.0f, 500.0f, 30.0f, 180.0f});
    decoration1_3D.push_back({240.0f, -100.0f, 600.0f, 28.0f, 225.0f});
    decoration1_3D.push_back({-140.0f, -100.0f, 700.0f, 29.0f, 270.0f});
    decoration1_3D.push_back({-170.0f, -100.0f, -110.0f, 28.0f, 315.0f});
    decoration1_3D.push_back({230.0f, -100.0f, -210.0f, 30.0f, 360.0f});
    decoration1_3D.push_back({-290.0f, -100.0f, -310.0f, 29.0f, 405.0f});
    decoration1_3D.push_back({170.0f, -100.0f, -410.0f, 27.0f, 450.0f});
    decoration1_3D.push_back({-330.0f, -100.0f, -510.0f, 30.0f, 495.0f});
    decoration1_3D.push_back({250.0f, -100.0f, -610.0f, 28.0f, 540.0f});

    // Decoração 2
    decoration2_3D.push_back({-200.0f, -100.0f, 150.0f, 26.0f, 30.0f});
    decoration2_3D.push_back({180.0f, -100.0f, 250.0f, 28.0f, 75.0f});
    decoration2_3D.push_back({-300.0f, -100.0f, 350.0f, 27.0f, 120.0f});
    decoration2_3D.push_back({200.0f, -100.0f, 450.0f, 25.0f, 165.0f});
    decoration2_3D.push_back({-260.0f, -100.0f, 550.0f, 28.0f, 210.0f});
    decoration2_3D.push_back({280.0f, -100.0f, 650.0f, 26.0f, 255.0f});
    decoration2_3D.push_back({-210.0f, -100.0f, -160.0f, 26.0f, 300.0f});
    decoration2_3D.push_back({190.0f, -100.0f, -260.0f, 28.0f, 345.0f});
    decoration2_3D.push_back({-310.0f, -100.0f, -360.0f, 27.0f, 390.0f});
    decoration2_3D.push_back({210.0f, -100.0f, -460.0f, 25.0f, 435.0f});
    decoration2_3D.push_back({-270.0f, -100.0f, -560.0f, 28.0f, 480.0f});

    // Decoração 3
    decoration3_3D.push_back({-180.0f, -100.0f, 180.0f, 30.0f, 60.0f});
    decoration3_3D.push_back({260.0f, -100.0f, 280.0f, 32.0f, 105.0f});
    decoration3_3D.push_back({-240.0f, -100.0f, 380.0f, 31.0f, 150.0f});
    decoration3_3D.push_back({140.0f, -100.0f, 480.0f, 29.0f, 195.0f});
    decoration3_3D.push_back({-340.0f, -100.0f, 580.0f, 32.0f, 240.0f});
    decoration3_3D.push_back({220.0f, -100.0f, 680.0f, 30.0f, 285.0f});
    decoration3_3D.push_back({-190.0f, -100.0f, -190.0f, 30.0f, 330.0f});
    decoration3_3D.push_back({270.0f, -100.0f, -290.0f, 32.0f, 375.0f});
    decoration3_3D.push_back({-250.0f, -100.0f, -390.0f, 31.0f, 420.0f});
    decoration3_3D.push_back({150.0f, -100.0f, -490.0f, 29.0f, 465.0f});
    decoration3_3D.push_back({-350.0f, -100.0f, -590.0f, 32.0f, 510.0f});
}

void loadLevel2Decorations(std::vector<DecoObject>& hexagons_3D,
                          std::vector<DecoObject>& flowers_3D,
                          std::vector<DecoObject>& plants_3D,
                          std::vector<DecoObject>& rocks_3D,
                          std::vector<DecoObject>& pines_3D,
                          std::vector<DecoObject>& trees_3D,
                          std::vector<DecoObject>& decoration1_3D,
                          std::vector<DecoObject>& decoration2_3D,
                          std::vector<DecoObject>& decoration3_3D) {
    // Hexágonos
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

    // Flores
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

    // Plantas
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

    // Rochas
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

    // Pinheiros
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
    pines_3D.push_back({-450.0f, -100.0f, 35.0f, 31.0f, 2285.0f});
    pines_3D.push_back({480.0f, -100.0f, 45.0f, 29.5f, 2290.0f});
    pines_3D.push_back({-550.0f, -100.0f, 120.0f, 30.0f, 2295.0f});
    pines_3D.push_back({600.0f, -100.0f, 200.0f, 31.5f, 2300.0f});
    pines_3D.push_back({-420.0f, -100.0f, 750.0f, 29.0f, 2305.0f});
    pines_3D.push_back({530.0f, -100.0f, 780.0f, 30.5f, 2310.0f});
    pines_3D.push_back({-25.0f, -100.0f, 15.0f, 28.0f, 2315.0f});
    pines_3D.push_back({35.0f, -100.0f, 25.0f, 29.0f, 2320.0f});
    pines_3D.push_back({-470.0f, -100.0f, -40.0f, 30.5f, 2325.0f});
    pines_3D.push_back({510.0f, -100.0f, -50.0f, 29.0f, 2330.0f});
    pines_3D.push_back({-580.0f, -100.0f, -130.0f, 31.0f, 2335.0f});
    pines_3D.push_back({620.0f, -100.0f, -210.0f, 30.0f, 2340.0f});
    pines_3D.push_back({-440.0f, -100.0f, -760.0f, 30.5f, 2345.0f});
    pines_3D.push_back({560.0f, -100.0f, -790.0f, 29.5f, 2350.0f});
    pines_3D.push_back({-30.0f, -100.0f, -18.0f, 28.5f, 2355.0f});
    pines_3D.push_back({40.0f, -100.0f, -28.0f, 29.5f, 2360.0f});

    // Árvores normais
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
    trees_3D.push_back({-500.0f, -100.0f, 60.0f, 37.0f, 2245.0f});
    trees_3D.push_back({520.0f, -100.0f, 80.0f, 35.5f, 2250.0f});
    trees_3D.push_back({-600.0f, -100.0f, 150.0f, 36.0f, 2255.0f});
    trees_3D.push_back({650.0f, -100.0f, 230.0f, 38.0f, 2260.0f});
    trees_3D.push_back({-480.0f, -100.0f, 770.0f, 37.5f, 2265.0f});
    trees_3D.push_back({570.0f, -100.0f, 800.0f, 36.5f, 2270.0f});
    trees_3D.push_back({-35.0f, -100.0f, 20.0f, 34.0f, 2275.0f});
    trees_3D.push_back({45.0f, -100.0f, 30.0f, 35.0f, 2280.0f});
    trees_3D.push_back({-520.0f, -100.0f, -70.0f, 37.0f, 2245.0f});
    trees_3D.push_back({540.0f, -100.0f, -90.0f, 36.0f, 2250.0f});
    trees_3D.push_back({-620.0f, -100.0f, -160.0f, 38.0f, 2255.0f});
    trees_3D.push_back({670.0f, -100.0f, -240.0f, 35.5f, 2260.0f});
    trees_3D.push_back({-500.0f, -100.0f, -780.0f, 36.5f, 2265.0f});
    trees_3D.push_back({590.0f, -100.0f, -810.0f, 37.5f, 2270.0f});
    trees_3D.push_back({-40.0f, -100.0f, -22.0f, 34.5f, 2275.0f});
    trees_3D.push_back({50.0f, -100.0f, -32.0f, 35.5f, 2280.0f});

    // Decoração 1
    decoration1_3D.push_back({-155.0f, -100.0f, 110.0f, 18.1f, 15.0f});
    decoration1_3D.push_back({225.0f, -100.0f, 210.0f, 19.4f, 60.0f});
    decoration1_3D.push_back({-275.0f, -100.0f, 310.0f, 18.5f, 105.0f});
    decoration1_3D.push_back({165.0f, -100.0f, 410.0f, 17.2f, 150.0f});
    decoration1_3D.push_back({-315.0f, -100.0f, 510.0f, 19.0f, 195.0f});
    decoration1_3D.push_back({245.0f, -100.0f, 610.0f, 17.8f, 240.0f});
    decoration1_3D.push_back({-135.0f, -100.0f, 710.0f, 18.5f, 285.0f});
    decoration1_3D.push_back({-165.0f, -100.0f, -120.0f, 18.1f, 330.0f});
    decoration1_3D.push_back({235.0f, -100.0f, -220.0f, 19.4f, 375.0f});
    decoration1_3D.push_back({-285.0f, -100.0f, -320.0f, 18.5f, 420.0f});
    decoration1_3D.push_back({175.0f, -100.0f, -420.0f, 17.2f, 465.0f});
    decoration1_3D.push_back({-325.0f, -100.0f, -520.0f, 19.0f, 510.0f});

    // Decoração 2
    decoration2_3D.push_back({-195.0f, -100.0f, 160.0f, 16.5f, 45.0f});
    decoration2_3D.push_back({185.0f, -100.0f, 260.0f, 17.8f, 90.0f});
    decoration2_3D.push_back({-295.0f, -100.0f, 360.0f, 17.2f, 135.0f});
    decoration2_3D.push_back({205.0f, -100.0f, 460.0f, 16.0f, 180.0f});
    decoration2_3D.push_back({-255.0f, -100.0f, 560.0f, 17.8f, 225.0f});
    decoration2_3D.push_back({285.0f, -100.0f, 660.0f, 16.5f, 270.0f});
    decoration2_3D.push_back({-205.0f, -100.0f, -170.0f, 16.5f, 315.0f});
    decoration2_3D.push_back({195.0f, -100.0f, -270.0f, 17.8f, 360.0f});
    decoration2_3D.push_back({-305.0f, -100.0f, -370.0f, 17.2f, 405.0f});
    decoration2_3D.push_back({215.0f, -100.0f, -470.0f, 16.0f, 450.0f});

    // Decoração 3
    decoration3_3D.push_back({-175.0f, -100.0f, 190.0f, 19.0f, 75.0f});
    decoration3_3D.push_back({265.0f, -100.0f, 290.0f, 20.2f, 120.0f});
    decoration3_3D.push_back({-235.0f, -100.0f, 390.0f, 19.8f, 165.0f});
    decoration3_3D.push_back({145.0f, -100.0f, 490.0f, 18.5f, 210.0f});
    decoration3_3D.push_back({-335.0f, -100.0f, 590.0f, 20.2f, 255.0f});
    decoration3_3D.push_back({225.0f, -100.0f, 690.0f, 19.0f, 300.0f});
    decoration3_3D.push_back({-185.0f, -100.0f, -200.0f, 19.0f, 345.0f});
    decoration3_3D.push_back({275.0f, -100.0f, -300.0f, 20.2f, 390.0f});
    decoration3_3D.push_back({-245.0f, -100.0f, -400.0f, 19.8f, 435.0f});
    decoration3_3D.push_back({155.0f, -100.0f, -500.0f, 18.5f, 480.0f});
}

void loadLevel3Decorations(std::vector<DecoObject>& hexagons_3D,
                          std::vector<DecoObject>& flowers_3D,
                          std::vector<DecoObject>& plants_3D,
                          std::vector<DecoObject>& rocks_3D,
                          std::vector<DecoObject>& pines_3D,
                          std::vector<DecoObject>& trees_3D,
                          std::vector<DecoObject>& decoration1_3D,
                          std::vector<DecoObject>& decoration2_3D,
                          std::vector<DecoObject>& decoration3_3D) {
    // Level 3: Decorações para cemitério/horror
    
    // Flores 
    flowers_3D.push_back({-220.0f, -100.0f, 80.0f, 32.4f, 0.0f});
    flowers_3D.push_back({-200.0f, -100.0f, 82.0f, 34.2f, 30.0f});
    flowers_3D.push_back({15.0f, -100.0f, 170.0f, 33.3f, 60.0f});
    flowers_3D.push_back({340.0f, -100.0f, 172.0f, 31.5f, 90.0f});
    flowers_3D.push_back({-170.0f, -100.0f, 260.0f, 34.2f, 120.0f});
    flowers_3D.push_back({-18.0f, -100.0f, 262.0f, 32.4f, 150.0f});
    flowers_3D.push_back({195.0f, -100.0f, 350.0f, 31.5f, 180.0f});
    flowers_3D.push_back({65.0f, -100.0f, 352.0f, 34.2f, 210.0f});
    flowers_3D.push_back({8.0f, -100.0f, 510.0f, 32.04f, 240.0f});
    flowers_3D.push_back({10.0f, -100.0f, 512.0f, 33.48f, 270.0f});
    flowers_3D.push_back({-15.0f, -100.0f, 640.0f, 32.76f, 300.0f});
    flowers_3D.push_back({-12.0f, -100.0f, 642.0f, 32.04f, 330.0f});
    flowers_3D.push_back({-230.0f, -100.0f, -90.0f, 32.4f, 360.0f});
    flowers_3D.push_back({-210.0f, -100.0f, -92.0f, 34.2f, 390.0f});
    flowers_3D.push_back({25.0f, -100.0f, -180.0f, 33.3f, 420.0f});
    flowers_3D.push_back({350.0f, -100.0f, -182.0f, 31.5f, 450.0f});
    flowers_3D.push_back({-180.0f, -100.0f, -270.0f, 34.2f, 480.0f});
    flowers_3D.push_back({-28.0f, -100.0f, -272.0f, 32.4f, 510.0f});
    flowers_3D.push_back({205.0f, -100.0f, -360.0f, 31.5f, 540.0f});
    flowers_3D.push_back({75.0f, -100.0f, -362.0f, 34.2f, 570.0f});
    flowers_3D.push_back({18.0f, -100.0f, -520.0f, 32.04f, 600.0f});
    flowers_3D.push_back({20.0f, -100.0f, -522.0f, 33.48f, 630.0f});
    flowers_3D.push_back({-5.0f, -100.0f, -650.0f, 32.76f, 660.0f});
    flowers_3D.push_back({-2.0f, -100.0f, -652.0f, 32.04f, 690.0f});

    // Plantas
    plants_3D.push_back({-130.0f, -100.0f, 100.0f, 36.0f, 15.0f});
    plants_3D.push_back({270.0f, -100.0f, 190.0f, 33.3f, 45.0f});
    plants_3D.push_back({-35.0f, -100.0f, 280.0f, 37.8f, 75.0f});
    plants_3D.push_back({115.0f, -100.0f, 360.0f, 31.5f, 105.0f});
    plants_3D.push_back({-285.0f, -100.0f, 440.0f, 36.0f, 135.0f});
    plants_3D.push_back({325.0f, -100.0f, 530.0f, 34.2f, 165.0f});
    plants_3D.push_back({-165.0f, -100.0f, 610.0f, 36.9f, 195.0f});
    plants_3D.push_back({205.0f, -100.0f, 690.0f, 35.1f, 225.0f});
    plants_3D.push_back({18.0f, -100.0f, 560.0f, 33.84f, 255.0f});
    plants_3D.push_back({-22.0f, -100.0f, 710.0f, 36.36f, 285.0f});
    plants_3D.push_back({-140.0f, -100.0f, -110.0f, 36.0f, 315.0f});
    plants_3D.push_back({280.0f, -100.0f, -200.0f, 33.3f, 345.0f});
    plants_3D.push_back({-45.0f, -100.0f, -290.0f, 37.8f, 375.0f});
    plants_3D.push_back({125.0f, -100.0f, -370.0f, 31.5f, 405.0f});
    plants_3D.push_back({-295.0f, -100.0f, -450.0f, 36.0f, 435.0f});
    plants_3D.push_back({335.0f, -100.0f, -540.0f, 34.2f, 465.0f});
    plants_3D.push_back({-175.0f, -100.0f, -620.0f, 36.9f, 495.0f});
    plants_3D.push_back({215.0f, -100.0f, -700.0f, 35.1f, 525.0f});
    plants_3D.push_back({12.0f, -100.0f, -260.0f, 34.56f, 555.0f});
    plants_3D.push_back({-18.0f, -100.0f, -410.0f, 37.44f, 585.0f});
    plants_3D.push_back({22.0f, -100.0f, -570.0f, 33.48f, 615.0f});
    plants_3D.push_back({-10.0f, -100.0f, -730.0f, 36.72f, 645.0f});

    // Rochas
    rocks_3D.push_back({-245.0f, -100.0f, 120.0f, 43.2f, 0.0f});
    rocks_3D.push_back({305.0f, -100.0f, 200.0f, 36.9f, 60.0f});
    rocks_3D.push_back({-225.0f, -100.0f, 300.0f, 46.8f, 120.0f});
    rocks_3D.push_back({345.0f, -100.0f, 380.0f, 40.5f, 180.0f});
    rocks_3D.push_back({-215.0f, -100.0f, 460.0f, 43.2f, 240.0f});
    rocks_3D.push_back({185.0f, -100.0f, 540.0f, 42.3f, 300.0f});
    rocks_3D.push_back({-375.0f, -100.0f, 620.0f, 45.9f, 360.0f});
    rocks_3D.push_back({95.0f, -100.0f, 700.0f, 38.7f, 420.0f});
    rocks_3D.push_back({-12.0f, -100.0f, 590.0f, 44.64f, 480.0f});
    rocks_3D.push_back({22.0f, -100.0f, 720.0f, 42.12f, 540.0f});
    rocks_3D.push_back({-255.0f, -100.0f, -130.0f, 43.2f, 600.0f});
    rocks_3D.push_back({315.0f, -100.0f, -210.0f, 36.9f, 660.0f});
    rocks_3D.push_back({-235.0f, -100.0f, -310.0f, 46.8f, 720.0f});
    rocks_3D.push_back({355.0f, -100.0f, -390.0f, 40.5f, 780.0f});
    rocks_3D.push_back({-225.0f, -100.0f, -470.0f, 43.2f, 840.0f});
    rocks_3D.push_back({195.0f, -100.0f, -550.0f, 42.3f, 900.0f});
    rocks_3D.push_back({-365.0f, -100.0f, -630.0f, 45.9f, 960.0f});
    rocks_3D.push_back({105.0f, -100.0f, -710.0f, 38.7f, 1020.0f});
    rocks_3D.push_back({15.0f, -100.0f, -190.0f, 45.36f, 1080.0f});
    rocks_3D.push_back({-20.0f, -100.0f, -340.0f, 47.52f, 1140.0f});
    rocks_3D.push_back({10.0f, -100.0f, -490.0f, 46.08f, 1200.0f});
    rocks_3D.push_back({-15.0f, -100.0f, -660.0f, 41.04f, 1260.0f});
    rocks_3D.push_back({18.0f, -100.0f, -750.0f, 46.8f, 1320.0f});

    // Pinheiros
    pines_3D.push_back({-105.0f, -100.0f, 70.0f, 28.0f, 0.0f});
    pines_3D.push_back({235.0f, -100.0f, 150.0f, 26.5f, 45.0f});
    pines_3D.push_back({-335.0f, -100.0f, 240.0f, 30.0f, 90.0f});
    pines_3D.push_back({175.0f, -100.0f, 330.0f, 28.5f, 135.0f});
    pines_3D.push_back({-265.0f, -100.0f, 410.0f, 26.5f, 180.0f});
    pines_3D.push_back({355.0f, -100.0f, 490.0f, 28.0f, 225.0f});
    pines_3D.push_back({-145.0f, -100.0f, 570.0f, 29.0f, 270.0f});
    pines_3D.push_back({295.0f, -100.0f, 650.0f, 27.5f, 315.0f});
    pines_3D.push_back({12.0f, -100.0f, 620.0f, 28.8f, 360.0f});
    pines_3D.push_back({-18.0f, -100.0f, 790.0f, 27.6f, 405.0f});
    pines_3D.push_back({-450.0f, -100.0f, 35.0f, 44.25f, 15.0f});
    pines_3D.push_back({480.0f, -100.0f, 45.0f, 40.5f, 30.0f});
    pines_3D.push_back({-550.0f, -100.0f, 120.0f, 42.75f, 60.0f});
    pines_3D.push_back({600.0f, -100.0f, 200.0f, 43.5f, 90.0f});
    pines_3D.push_back({-420.0f, -100.0f, 750.0f, 42.0f, 120.0f});
    pines_3D.push_back({530.0f, -100.0f, 780.0f, 41.25f, 150.0f});
    pines_3D.push_back({-25.0f, -100.0f, 15.0f, 39.0f, 5.0f});
    pines_3D.push_back({35.0f, -100.0f, 25.0f, 40.5f, 10.0f});
    pines_3D.push_back({-115.0f, -100.0f, -80.0f, 42.0f, 450.0f});
    pines_3D.push_back({245.0f, -100.0f, -160.0f, 39.75f, 495.0f});
    pines_3D.push_back({-345.0f, -100.0f, -250.0f, 45.0f, 540.0f});
    pines_3D.push_back({185.0f, -100.0f, -340.0f, 42.75f, 585.0f});
    pines_3D.push_back({-275.0f, -100.0f, -420.0f, 39.75f, 630.0f});
    pines_3D.push_back({365.0f, -100.0f, -500.0f, 42.0f, 675.0f});
    pines_3D.push_back({-155.0f, -100.0f, -580.0f, 43.5f, 720.0f});
    pines_3D.push_back({305.0f, -100.0f, -660.0f, 41.25f, 765.0f});
    pines_3D.push_back({-12.0f, -100.0f, -210.0f, 42.3f, 810.0f});
    pines_3D.push_back({18.0f, -100.0f, -360.0f, 44.4f, 855.0f});
    pines_3D.push_back({-15.0f, -100.0f, -510.0f, 40.8f, 900.0f});
    pines_3D.push_back({10.0f, -100.0f, -690.0f, 43.8f, 945.0f});
    pines_3D.push_back({-470.0f, -100.0f, -40.0f, 43.5f, 975.0f});
    pines_3D.push_back({510.0f, -100.0f, -50.0f, 42.0f, 1005.0f});
    pines_3D.push_back({-580.0f, -100.0f, -130.0f, 41.25f, 1035.0f});
    pines_3D.push_back({620.0f, -100.0f, -210.0f, 42.75f, 1065.0f});
    pines_3D.push_back({-440.0f, -100.0f, -760.0f, 44.25f, 1095.0f});
    pines_3D.push_back({560.0f, -100.0f, -790.0f, 42.0f, 1125.0f});
    pines_3D.push_back({-30.0f, -100.0f, -18.0f, 39.75f, 1150.0f});
    pines_3D.push_back({40.0f, -100.0f, -28.0f, 41.25f, 1175.0f});

    // Árvores
    trees_3D.push_back({-200.0f, -100.0f, 130.0f, 48.0f, 30.0f});
    trees_3D.push_back({280.0f, -100.0f, 220.0f, 45.0f, 75.0f});
    trees_3D.push_back({-350.0f, -100.0f, 310.0f, 51.0f, 120.0f});
    trees_3D.push_back({140.0f, -100.0f, 400.0f, 48.0f, 165.0f});
    trees_3D.push_back({-220.0f, -100.0f, 480.0f, 45.0f, 210.0f});
    trees_3D.push_back({370.0f, -100.0f, 560.0f, 48.0f, 255.0f});
    trees_3D.push_back({-120.0f, -100.0f, 640.0f, 49.5f, 300.0f});
    trees_3D.push_back({260.0f, -100.0f, 720.0f, 46.5f, 345.0f});
    trees_3D.push_back({15.0f, -100.0f, 650.0f, 48.6f, 390.0f});
    trees_3D.push_back({-500.0f, -100.0f, 60.0f, 50.25f, 20.0f});
    trees_3D.push_back({520.0f, -100.0f, 80.0f, 47.25f, 50.0f});
    trees_3D.push_back({-600.0f, -100.0f, 150.0f, 48.0f, 80.0f});
    trees_3D.push_back({650.0f, -100.0f, 230.0f, 51.0f, 110.0f});
    trees_3D.push_back({-480.0f, -100.0f, 770.0f, 49.5f, 140.0f});
    trees_3D.push_back({570.0f, -100.0f, 800.0f, 48.75f, 170.0f});
    trees_3D.push_back({-35.0f, -100.0f, 20.0f, 45.0f, 8.0f});
    trees_3D.push_back({45.0f, -100.0f, 30.0f, 46.5f, 12.0f});
    trees_3D.push_back({-210.0f, -100.0f, -140.0f, 48.0f, 435.0f});
    trees_3D.push_back({290.0f, -100.0f, -230.0f, 45.0f, 480.0f});
    trees_3D.push_back({-360.0f, -100.0f, -320.0f, 51.0f, 525.0f});
    trees_3D.push_back({150.0f, -100.0f, -410.0f, 48.0f, 570.0f});
    trees_3D.push_back({-230.0f, -100.0f, -490.0f, 45.0f, 615.0f});
    trees_3D.push_back({380.0f, -100.0f, -570.0f, 48.0f, 660.0f});
    trees_3D.push_back({-130.0f, -100.0f, -650.0f, 49.5f, 705.0f});
    trees_3D.push_back({270.0f, -100.0f, -730.0f, 46.5f, 750.0f});
    trees_3D.push_back({-15.0f, -100.0f, -230.0f, 49.2f, 795.0f});
    trees_3D.push_back({20.0f, -100.0f, -380.0f, 50.4f, 840.0f});
    trees_3D.push_back({-12.0f, -100.0f, -530.0f, 46.2f, 885.0f});
    trees_3D.push_back({8.0f, -100.0f, -700.0f, 49.8f, 930.0f});
    trees_3D.push_back({-520.0f, -100.0f, -70.0f, 49.5f, 960.0f});
    trees_3D.push_back({540.0f, -100.0f, -90.0f, 48.0f, 990.0f});
    trees_3D.push_back({-620.0f, -100.0f, -160.0f, 51.0f, 1020.0f});
    trees_3D.push_back({670.0f, -100.0f, -240.0f, 47.25f, 1050.0f});
    trees_3D.push_back({-500.0f, -100.0f, -780.0f, 48.75f, 1080.0f});
    trees_3D.push_back({590.0f, -100.0f, -810.0f, 50.25f, 1110.0f});
    trees_3D.push_back({-40.0f, -100.0f, -22.0f, 45.75f, 1140.0f});
    trees_3D.push_back({50.0f, -100.0f, -32.0f, 47.25f, 1170.0f});

    // Decoração 1
    decoration1_3D.push_back({-150.0f, -100.0f, 120.0f, 42.0f, 20.0f});
    decoration1_3D.push_back({230.0f, -100.0f, 220.0f, 45.0f, 65.0f});
    decoration1_3D.push_back({-270.0f, -100.0f, 320.0f, 43.5f, 110.0f});
    decoration1_3D.push_back({170.0f, -100.0f, 420.0f, 40.5f, 155.0f});
    decoration1_3D.push_back({-310.0f, -100.0f, 520.0f, 45.0f, 200.0f});
    decoration1_3D.push_back({250.0f, -100.0f, 620.0f, 42.0f, 245.0f});
    decoration1_3D.push_back({-130.0f, -100.0f, 720.0f, 43.5f, 290.0f});
    decoration1_3D.push_back({50.0f, -100.0f, 130.0f, 42.75f, 10.0f});
    decoration1_3D.push_back({-100.0f, -100.0f, 240.0f, 44.25f, 55.0f});
    decoration1_3D.push_back({300.0f, -100.0f, 340.0f, 41.25f, 100.0f});
    decoration1_3D.push_back({-350.0f, -100.0f, 450.0f, 42.0f, 145.0f});
    decoration1_3D.push_back({100.0f, -100.0f, 550.0f, 43.5f, 190.0f});
    decoration1_3D.push_back({-200.0f, -100.0f, 660.0f, 41.25f, 235.0f});
    decoration1_3D.push_back({350.0f, -100.0f, 760.0f, 42.75f, 280.0f});
    decoration1_3D.push_back({-160.0f, -100.0f, -130.0f, 42.0f, 335.0f});
    decoration1_3D.push_back({240.0f, -100.0f, -230.0f, 45.0f, 380.0f});
    decoration1_3D.push_back({-280.0f, -100.0f, -330.0f, 43.5f, 425.0f});
    decoration1_3D.push_back({180.0f, -100.0f, -430.0f, 40.5f, 470.0f});
    decoration1_3D.push_back({-320.0f, -100.0f, -530.0f, 45.0f, 515.0f});
    decoration1_3D.push_back({260.0f, -100.0f, -630.0f, 42.0f, 560.0f});
    decoration1_3D.push_back({60.0f, -100.0f, -140.0f, 42.75f, 325.0f});
    decoration1_3D.push_back({-110.0f, -100.0f, -250.0f, 44.25f, 370.0f});
    decoration1_3D.push_back({310.0f, -100.0f, -350.0f, 41.25f, 415.0f});
    decoration1_3D.push_back({-340.0f, -100.0f, -460.0f, 42.0f, 460.0f});
    decoration1_3D.push_back({110.0f, -100.0f, -560.0f, 43.5f, 505.0f});
    decoration1_3D.push_back({-190.0f, -100.0f, -670.0f, 41.25f, 550.0f});

    // Decoração 2
    decoration2_3D.push_back({-190.0f, -100.0f, 170.0f, 39.0f, 50.0f});
    decoration2_3D.push_back({190.0f, -100.0f, 270.0f, 42.0f, 95.0f});
    decoration2_3D.push_back({-290.0f, -100.0f, 370.0f, 40.5f, 140.0f});
    decoration2_3D.push_back({210.0f, -100.0f, 470.0f, 37.5f, 185.0f});
    decoration2_3D.push_back({-250.0f, -100.0f, 570.0f, 42.0f, 230.0f});
    decoration2_3D.push_back({290.0f, -100.0f, 670.0f, 39.0f, 275.0f});
    decoration2_3D.push_back({30.0f, -100.0f, 180.0f, 39.75f, 40.0f});
    decoration2_3D.push_back({-120.0f, -100.0f, 290.0f, 41.25f, 85.0f});
    decoration2_3D.push_back({320.0f, -100.0f, 390.0f, 38.25f, 130.0f});
    decoration2_3D.push_back({-360.0f, -100.0f, 500.0f, 39.0f, 175.0f});
    decoration2_3D.push_back({120.0f, -100.0f, 600.0f, 40.5f, 220.0f});
    decoration2_3D.push_back({-220.0f, -100.0f, 710.0f, 38.25f, 265.0f});
    decoration2_3D.push_back({-200.0f, -100.0f, -180.0f, 39.0f, 320.0f});
    decoration2_3D.push_back({200.0f, -100.0f, -280.0f, 42.0f, 365.0f});
    decoration2_3D.push_back({-300.0f, -100.0f, -380.0f, 40.5f, 410.0f});
    decoration2_3D.push_back({220.0f, -100.0f, -480.0f, 37.5f, 455.0f});
    decoration2_3D.push_back({-260.0f, -100.0f, -580.0f, 42.0f, 500.0f});
    decoration2_3D.push_back({40.0f, -100.0f, -190.0f, 39.75f, 310.0f});
    decoration2_3D.push_back({-130.0f, -100.0f, -300.0f, 41.25f, 355.0f});
    decoration2_3D.push_back({330.0f, -100.0f, -400.0f, 38.25f, 400.0f});
    decoration2_3D.push_back({-350.0f, -100.0f, -510.0f, 39.0f, 445.0f});
    decoration2_3D.push_back({130.0f, -100.0f, -610.0f, 40.5f, 490.0f});

    // Decoração 3 
    decoration3_3D.push_back({-170.0f, -100.0f, 200.0f, 45.0f, 80.0f});
    decoration3_3D.push_back({270.0f, -100.0f, 300.0f, 48.0f, 125.0f});
    decoration3_3D.push_back({-230.0f, -100.0f, 400.0f, 46.5f, 170.0f});
    decoration3_3D.push_back({150.0f, -100.0f, 500.0f, 43.5f, 215.0f});
    decoration3_3D.push_back({-330.0f, -100.0f, 600.0f, 48.0f, 260.0f});
    decoration3_3D.push_back({230.0f, -100.0f, 700.0f, 45.0f, 305.0f});
    decoration3_3D.push_back({70.0f, -100.0f, 210.0f, 45.75f, 70.0f});
    decoration3_3D.push_back({-140.0f, -100.0f, 320.0f, 47.25f, 115.0f});
    decoration3_3D.push_back({340.0f, -100.0f, 420.0f, 44.25f, 160.0f});
    decoration3_3D.push_back({-370.0f, -100.0f, 530.0f, 45.0f, 205.0f});
    decoration3_3D.push_back({140.0f, -100.0f, 630.0f, 46.5f, 250.0f});
    decoration3_3D.push_back({-240.0f, -100.0f, 740.0f, 44.25f, 295.0f});
    decoration3_3D.push_back({-180.0f, -100.0f, -210.0f, 45.0f, 350.0f});
    decoration3_3D.push_back({280.0f, -100.0f, -310.0f, 48.0f, 395.0f});
    decoration3_3D.push_back({-240.0f, -100.0f, -410.0f, 46.5f, 440.0f});
    decoration3_3D.push_back({160.0f, -100.0f, -510.0f, 43.5f, 485.0f});
    decoration3_3D.push_back({-340.0f, -100.0f, -610.0f, 48.0f, 530.0f});
    decoration3_3D.push_back({80.0f, -100.0f, -220.0f, 45.75f, 340.0f});
    decoration3_3D.push_back({-150.0f, -100.0f, -330.0f, 47.25f, 385.0f});
    decoration3_3D.push_back({350.0f, -100.0f, -430.0f, 44.25f, 430.0f});
    decoration3_3D.push_back({-360.0f, -100.0f, -540.0f, 45.0f, 475.0f});
    decoration3_3D.push_back({150.0f, -100.0f, -640.0f, 46.5f, 520.0f});
}
