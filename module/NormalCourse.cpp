/**
 *  @file   NormalCourse.h
 *  @brief  NormalCourseを走る
 *  @author Tatsumi0000, Takahiro55555
 */
#include "NormalCourse.h"

/** コンストラクタ
 *  @param controller [コントローラのインスタンス]
 *  @param isLeftCourse_ [エッジがどっちかtrueがLeftコース]
 *  @param targetBrightness_ [カラーセンサーの目標値]
 */
NormalCourse::NormalCourse(Controller& controller_, bool isLeftCourse_, int targetBrightness_)
  : controller(controller_), isLeftCourse(isLeftCourse_), targetBrightness(targetBrightness_)
{
}

/**
 * 左エッジ，右エッジを切り替える．
 * @param isLeftCourse_ [Leftコースである場合True]
 */
void NormalCourse::setIsLeftCourse(bool isLeftCourse_)
{
  isLeftCourse = isLeftCourse_;
}

/**
 * 実際にNormalコースを走る．
 * ここにパラメータを頑張って書く．
 */
void NormalCourse::runNormalCourse()
{
  printf("runNormalCourse\n");
  // 配列の個数
  constexpr int baseSpeed = 100;

  // LコースならLコースのPID値を採用する。RコースならRコースのPID値を採用する。
  std::string filename;
  if(isLeftCourse) {
    filename = L_PID_FILE_NAME;
  } else {
    filename = R_PID_FILE_NAME;
  }
  std::ifstream ifs(filename);

  std::string line;
  getline(ifs, line);
  if(initCsvParameters(line) != 0) {
    printf("[Error] CSVファイルのヘッダが正しくありません\n");
    return;
  }

  int counter = 2;
  while(getline(ifs, line)) {
    if(csvLineToNormalCourseProperty(line, baseSpeed) != 0) {
      printf("[Error] CSVファイルのパラメータ部分が正しくありません(%d 行目)\n", counter);
      return;
    }
    counter++;
  }

  LineTracer lineTracer(controller, targetBrightness, isLeftCourse);
  for(const auto& ncp : normalCoursePropertys) {
    lineTracer.run(ncp);
    // １区間終わるごとに音を奏でる．
    controller.speakerPlayToneFS6(100);
  }
}

/**
 * 現在のisLeftCourse（エッジ）を返すゲッター
 */
bool NormalCourse::getIsLeftCourse() const
{
  return isLeftCourse;
}

/**
 * targetBrightness（カラーセンサの目標値）を返すゲッター
 */
int NormalCourse::getTargetBrightness() const
{
  return targetBrightness;
}

std::vector<std::string> NormalCourse::split(std::string& input, char delimiter)
{
  // コピペ元: CVTECH.cc「C++でCSVファイルを読み込む」
  // URL: https://cvtech.cc/readcsv/

  std::istringstream stream(input);
  std::string field;
  std::vector<std::string> result;
  bool endIsEmpty = false;
  endIsEmpty = input[input.size()-1] == delimiter;

  while(getline(stream, field, delimiter)) {
    // 文字列の前後にある半角スペースを削除する
    result.push_back(std::regex_replace(field, std::regex("(^ +)|( +$)"), ""));
  }

  // 末尾がデリミタ文字で終わっている場合は空文字列を挿入する
  if(endIsEmpty)
  {
    result.push_back("");
  }

  return result;
}

int NormalCourse::initCsvParameters(std::string& headerLine)
{
  // CSV_DELIMITERにを参考に文字列を分割
  std::vector<std::string> header = split(headerLine, CSV_DELIMITER);

  // リセット
  targetDistanceCol = -1;
  targetSpeedCol = -1;
  curvatureCol = -1;
  turnPidPCol = -1;
  turnPidICol = -1;
  turnPidDCol = -1;
  headerColNum = header.size();  // ヘッダーの列数を取得しメンバ変数に格納

  for(int i = 0; i < headerColNum; i++) {
    if(PID_FILE_HEADER_TARGET_DISTANCE.compare(header[i]) == 0) {
      targetDistanceCol = i;
    } else if(PID_FILE_HEADER_TARGET_SPEED.compare(header[i]) == 0) {
      targetSpeedCol = i;
    } else if(PID_FILE_HEADER_CURVATURE.compare(header[i]) == 0) {
      curvatureCol = i;
    } else if(PID_FILE_HEADER_TURN_PID_P.compare(header[i]) == 0) {
      turnPidPCol = i;
    } else if(PID_FILE_HEADER_TURN_PID_I.compare(header[i]) == 0) {
      turnPidICol = i;
    } else if(PID_FILE_HEADER_TURN_PID_D.compare(header[i]) == 0) {
      turnPidDCol = i;
    }
  }

  // 全てのパラメータの列番号が分かっていない場合はエラー
  if(targetDistanceCol < 0 || targetSpeedCol < 0 || curvatureCol < 0 || turnPidPCol < 0
     || turnPidICol < 0 || turnPidDCol < 0) {
    return 1;
  }

  // normalCoursePropertys を空にする
  normalCoursePropertys.clear();

  return 0;
}

// HACK: 本来数値が入るべき箇所に数値以外の文字列が入っていても知らぬ顔で動作してしまう
int NormalCourse::csvLineToNormalCourseProperty(std::string& parameterLine, int baseSpeed)
{
  // NormalCourseProperty 関係
  int targetDistance = -1;
  int targetSpeed = -1;
  double curvature = -1;
  double Kp = -1;
  double Ki = -1;
  double Kd = -1;

  // CSV_DELIMITERにを参考に文字列を分割
  std::vector<std::string> parameter = split(parameterLine, CSV_DELIMITER);

  // ヘッダー行の列数と現在の行の列数が異なる場合エラー
  if(headerColNum != int(parameter.size())) {
    return 1;
  }

  // 各列番号の確認
  if(targetDistanceCol < 0) {
    return 1;
  }
  if(targetSpeedCol < 0) {
    return 1;
  }
  if(curvatureCol < 0) {
    return 1;
  }
  if(turnPidPCol < 0) {
    return 1;
  }
  if(turnPidICol < 0) {
    return 1;
  }
  if(turnPidDCol < 0) {
    return 1;
  }

  // 各値の確認
  if(parameter[targetDistanceCol].length() < 1) {
    return 1;
  }
  if(parameter[targetSpeedCol].length() < 1) {
    return 1;
  }
  if(parameter[curvatureCol].length() < 1) {
    return 1;
  }
  if(parameter[turnPidPCol].length() < 1) {
    return 1;
  }
  if(parameter[turnPidICol].length() < 1) {
    return 1;
  }
  if(parameter[turnPidDCol].length() < 1) {
    return 1;
  }

  // 型変換と変数への代入
  std::istringstream(parameter[targetDistanceCol]) >> targetDistance;
  std::istringstream(parameter[targetSpeedCol]) >> targetSpeed;
  std::istringstream(parameter[curvatureCol]) >> curvature;
  std::istringstream(parameter[turnPidPCol]) >> Kp;
  std::istringstream(parameter[turnPidICol]) >> Ki;
  std::istringstream(parameter[turnPidDCol]) >> Kd;

  NormalCourseProperty normalCourseProperty
      = NormalCourseProperty{ targetDistance, targetSpeed, curvature, { Kp, Ki, Kd } };
  normalCoursePropertys.push_back(normalCourseProperty);
  return 0;
}