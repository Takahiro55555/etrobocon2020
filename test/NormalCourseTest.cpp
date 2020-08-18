/**
 *  @file   NormalCourseTest.cpp
 *  @brief  NormalCourseクラスのテストファイル
 *  @author Tatsumi0000
 */

#include "NormalCourse.h"
#include <gtest/gtest.h>
#include <array>

namespace etrobocon2020_test {
  TEST(NormalCourse, NormalCourse_init)
  {
    Controller controller;
    // カラーセンサーの目標値
    int targetBrightness = 90;
    // Leftコースである場合True
    bool isLeftCourse = true;
    // カラーセンサーの目標値
    int targetBrightnessExpected = 90;
    NormalCourse normalCourse(controller, isLeftCourse, targetBrightness);
    ASSERT_TRUE(normalCourse.getIsLeftCourse());
    ASSERT_EQ(targetBrightnessExpected, normalCourse.getTargetBrightness());
  }

  //エッジの切替メソッドのテスト
  TEST(NormalCourse, setIsLeftCourse)
  {
    Controller controller;
    // カラーセンサーの目標値
    int targetBrightness = 100;
    // Leftコースである場合True
    bool isLeftCourse = false;
    NormalCourse normalCourse(controller, isLeftCourse, targetBrightness);
    normalCourse.setIsLeftCourse(true);
    ASSERT_TRUE(normalCourse.getIsLeftCourse());
  }

  // セットしたエッジを返すゲッター
  TEST(NormalCourse, getIsLeftCourse)
  {
    Controller controller;
    // カラーセンサーの目標値
    int targetBrightness = 100;
    // Leftコースである場合True
    bool isLeftCourse = false;
    NormalCourse normalCourse(controller, isLeftCourse, targetBrightness);
    ASSERT_FALSE(normalCourse.getIsLeftCourse());
  }

  // セットした目標のカラーセンサ値を返すゲッター
  TEST(NormalCourse, getTargetBrightness)
  {
    Controller controller;
    // カラーセンサーの目標値
    int targetBrightness = 600;
    // Leftコースである場合True
    bool isLeftCourse = true;
    // 期待される出力
    int expected = 600;
    NormalCourse normalCourse(controller, isLeftCourse, targetBrightness);
    ASSERT_EQ(expected, normalCourse.getTargetBrightness());
  }

  // 文字列分割に関するテスト
  TEST(NormalCourse, split)
  {
    Controller controller;
    // カラーセンサーの目標値
    int targetBrightness = 600;
    // Leftコースである場合True
    bool isLeftCourse = true;
    NormalCourse normalCourse(controller, isLeftCourse, targetBrightness);

    const char CSV_DELIMITER = ',';
    std::string input;
    std::vector<std::string> expected;
    
    // ケース1、通常
    input = "targetDistance,targetSpeed,curvature,turnP,turnI,turnD";
    expected = {"targetDistance","targetSpeed","curvature","turnP","turnI","turnD"};
    ASSERT_EQ(expected, normalCourse.split(input, CSV_DELIMITER));

    // ケース2、スペースの自動削除その1
    input = "      targetDistance,targetSpeed    ,curvature,   turnP   ,turnI,turnD";
    expected = {"targetDistance","targetSpeed","curvature","turnP","turnI","turnD"};
    ASSERT_EQ(expected, normalCourse.split(input, CSV_DELIMITER));

    // ケース3、末尾列が空
    input = "targetDistance,targetSpeed,curvature,turnP,turnI,turnD,";
    expected = {"targetDistance","targetSpeed","curvature","turnP","turnI","turnD",""};
    ASSERT_EQ(expected, normalCourse.split(input, CSV_DELIMITER));

    // ケース4、末尾列が半角スペース（スペースの自動削除その2）
    input = "targetDistance,targetSpeed,curvature,turnP,turnI,turnD, ";
    expected = {"targetDistance","targetSpeed","curvature","turnP","turnI","turnD",""};
    ASSERT_EQ(expected, normalCourse.split(input, CSV_DELIMITER));
  
    // ケース5、途中列が空、頭注列が半角スペース（スペースの自動削除その3）
    input = "targetDistance,,curvature, ,turnI,turnD";
    expected = {"targetDistance","","curvature","","turnI","turnD"};
    ASSERT_EQ(expected, normalCourse.split(input, CSV_DELIMITER));

    // ケース6、全ての列が空
    input = ",,,,,";
    expected = {"","","","","",""};
    ASSERT_EQ(expected, normalCourse.split(input, CSV_DELIMITER));

    // ケース6、全ての列が半角スペース（スペースの自動削除その4）
    input = " , ,           , ,              , ";
    expected = {"","","","","",""};
    ASSERT_EQ(expected, normalCourse.split(input, CSV_DELIMITER));
  }

  // CSVファイル読み込みのための初期化関数のテスト
  TEST(NormalCourse, initCsvParameters)
  {
    Controller controller;
    // カラーセンサーの目標値
    int targetBrightness = 600;
    // Leftコースである場合True
    bool isLeftCourse = true;
    NormalCourse normalCourse(controller, isLeftCourse, targetBrightness);

    std::string input;
    int expected;
    
    // ケース01、通常
    input = "targetDistance,targetSpeed,curvature,turnP,turnI,turnD";
    expected = 0;
    ASSERT_EQ(expected, normalCourse.initCsvParameters(input));

    // ケース02、列の順序交換
    input = "targetSpeed,targetDistance,turnI,curvature,turnP,turnD";
    expected = 0;
    ASSERT_EQ(expected, normalCourse.initCsvParameters(input));

    // ケース03、不必要な列の存在その1
    input = "targetDistance,targetSpeed,curvature,turnP,turnI,turnD,NOTE";
    expected = 0;
    ASSERT_EQ(expected, normalCourse.initCsvParameters(input));

    // ケース04、不必要な列の存在その2
    input = "targetDistance,targetSpeed,COMMENT,curvature,turnP,turnI,turnD,NOTE";
    expected = 0;
    ASSERT_EQ(expected, normalCourse.initCsvParameters(input));

    // ケース05、不必要な空の列の存在その1
    input = "targetDistance,targetSpeed,curvature,turnP,turnI,turnD,";
    expected = 0;
    ASSERT_EQ(expected, normalCourse.initCsvParameters(input));

    // ケース06、不必要な空の列の存在その2
    input = "targetDistance,,targetSpeed,curvature,turnP,turnI,turnD,";
    expected = 0;
    ASSERT_EQ(expected, normalCourse.initCsvParameters(input));

    // ケース07、列名が正しくないその1
    input = "target_distance,targetSpeed,curvature,turnP,turnI,turnD";
    expected = 0;
    ASSERT_NE(expected, normalCourse.initCsvParameters(input));

    // ケース08、列名が正しくないその2
    input = "targetDistance,targetSSpeed,curvature,turnP,turnI,turnD";
    expected = 0;
    ASSERT_NE(expected, normalCourse.initCsvParameters(input));

    // ケース09、列名が正しくないその3
    input = "targetDistance,targetSpeed,Curvature,turnP,turnI,turnD";
    expected = 0;
    ASSERT_NE(expected, normalCourse.initCsvParameters(input));

    // ケース10、列名が正しくないその4
    input = "targetDistance,targetSpeed,curvature,turn_p,turnI,turnD";
    expected = 0;
    ASSERT_NE(expected, normalCourse.initCsvParameters(input));

    // ケース11、列名が正しくないその5
    input = "targetDistance,targetSpeed,curvature,turnP,turn_I,turnD";
    expected = 0;
    ASSERT_NE(expected, normalCourse.initCsvParameters(input));

    // ケース12列名が正しくないその6
    input = "targetDistance,targetSpeed,curvature,turnP,turnI,TurnD";
    expected = 0;
    ASSERT_NE(expected, normalCourse.initCsvParameters(input));

    // ケース13、列名が正しくないその7、半角スペース以外の文字に挟まれている半角スペースが削除されないことの確認
    input = "targetDistance,target Speed,curvature,turnP,turnI,turnD";
    expected = 0;
    ASSERT_NE(expected, normalCourse.initCsvParameters(input));

    // ケース14、列名の不足
    input = "targetDistance,target Speed,curvature,turnP,turnI";
    expected = 0;
    ASSERT_NE(expected, normalCourse.initCsvParameters(input));
  }

  TEST(NormalCourse, csvLineToNormalCourseProperty)
  {
    Controller controller;
    // カラーセンサーの目標値
    int targetBrightness = 600;
    // Leftコースである場合True
    bool isLeftCourse = true;
    // 基準となるスピード
    int baseSpeed = 100;
    NormalCourse normalCourse(controller, isLeftCourse, targetBrightness);
    std::string input, header;
    int expected;

    // ケース01、通常
    header = "targetDistance,targetSpeed,curvature,turnP,turnI,turnD";
    normalCourse.initCsvParameters(header);
    input = "820,0,0.0,0.1,0.005,0.01";
    expected = 0;
    ASSERT_EQ(expected, normalCourse.csvLineToNormalCourseProperty(input, baseSpeed));

    // ケース02、通常
    header = "targetDistance,targetSpeed,curvature,turnP,turnI,turnD";
    normalCourse.initCsvParameters(header);
    input = "820,-10,0.0,0.1,0.005,0.01";
    expected = 0;
    ASSERT_EQ(expected, normalCourse.csvLineToNormalCourseProperty(input, baseSpeed));

    // ケース03、不要な複数列の存在
    header = "targetDistance,NOTE,targetSpeed,NOTE,curvature,NOTE,turnP,NOTE,turnI,NOTE,turnD,NOTE";
    normalCourse.initCsvParameters(header);
    input = "820,関係ない列,-10,関係ない列,0.0,関係ない列,0.1,関係ない列,0.005,関係ない列,0.01,関係ない列";
    expected = 0;
    ASSERT_EQ(expected, normalCourse.csvLineToNormalCourseProperty(input, baseSpeed));

    // ケース04、不要な列の存在その1、複数行の入力
    header = "targetDistance,targetSpeed,curvature,turnP,turnI,turnD,COMMENT";
    normalCourse.initCsvParameters(header);
    for(int i=1; i < 100; i++)
    {
      input = std::to_string(i*100+i) + ",-10,0.0,0.1,0.005,0.01,第" + std::to_string(i) + "区間";
      expected = 0;
      ASSERT_EQ(expected, normalCourse.csvLineToNormalCourseProperty(input, baseSpeed));
    }

    // ケース05、データ行の列数不足（境界値）
    header = "targetDistance,targetSpeed,curvature,turnP,turnI,turnD";
    normalCourse.initCsvParameters(header);
    input = "820,-10,0.1,0.005,0.01";
    expected = 0;
    ASSERT_NE(expected, normalCourse.csvLineToNormalCourseProperty(input, baseSpeed));

    // ケース06、データ行の列数過多（境界値）
    header = "targetDistance,targetSpeed,curvature,turnP,turnI,turnD";
    normalCourse.initCsvParameters(header);
    input = "820,-10,0.0,0.1,0.005,0.01,-100000000000000000";
    expected = 0;
    ASSERT_NE(expected, normalCourse.csvLineToNormalCourseProperty(input, baseSpeed));

    // ケース07、データ行の一部列が空
    header = "targetDistance,targetSpeed,curvature,turnP,turnI,turnD";
    normalCourse.initCsvParameters(header);
    input = ",-10,0.2,0.1,0.005,0.01";
    expected = 0;
    ASSERT_NE(expected, normalCourse.csvLineToNormalCourseProperty(input, baseSpeed));

    // ケース08、データ行の一部列が空
    header = "targetDistance,targetSpeed,curvature,turnP,turnI,turnD";
    normalCourse.initCsvParameters(header);
    input = "10000,,0.2,0.1,0.005,0.01";
    expected = 0;
    ASSERT_NE(expected, normalCourse.csvLineToNormalCourseProperty(input, baseSpeed));
    
    // ケース09、データ行の一部列が空
    header = "targetDistance,targetSpeed,curvature,turnP,turnI,turnD";
    normalCourse.initCsvParameters(header);
    input = "10000,-10,,0.1,0.005,0.01";
    expected = 0;
    ASSERT_NE(expected, normalCourse.csvLineToNormalCourseProperty(input, baseSpeed));

    // ケース10、データ行の一部列が空
    header = "targetDistance,targetSpeed,curvature,turnP,turnI,turnD";
    normalCourse.initCsvParameters(header);
    input = "10000,-10,0.2,,0.005,0.01";
    expected = 0;
    ASSERT_NE(expected, normalCourse.csvLineToNormalCourseProperty(input, baseSpeed));

    // ケース11、データ行の一部列が空
    header = "targetDistance,targetSpeed,curvature,turnP,turnI,turnD";
    normalCourse.initCsvParameters(header);
    input = "10000,-10,0.2,0.1,,0.01";
    expected = 0;
    ASSERT_NE(expected, normalCourse.csvLineToNormalCourseProperty(input, baseSpeed));

    // ケース12、データ行の一部列が空
    header = "targetDistance,targetSpeed,curvature,turnP,turnI,turnD";
    normalCourse.initCsvParameters(header);
    input = "10000,-10,0.2,0.1,0.005,";
    expected = 0;
    ASSERT_NE(expected, normalCourse.csvLineToNormalCourseProperty(input, baseSpeed));

    // ケース13、データ行の一部列が半角スペース
    header = "targetDistance,targetSpeed,curvature,turnP,turnI,turnD";
    normalCourse.initCsvParameters(header);
    input = "10000, ,0.2,0.1,0.005,0.01";
    expected = 0;
    ASSERT_NE(expected, normalCourse.csvLineToNormalCourseProperty(input, baseSpeed));
  }

}  // namespace etrobocon2020_test
