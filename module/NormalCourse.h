/**
 *  @file   NormalCourse.h
 *  @brief  NormalCourseを走る
 *  @author Tatsumi0000
 */
#ifndef NORMALCOURSE_H
#define NORMALCOURSE_H
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include "LineTracer.h"
#include "Controller.h"

class NormalCourse {
 public:
  /** コンストラクタ
   *  @param controller [コントローラのインスタンス]
   *  @param isLeftCourse_ [エッジがどっちかtrueがLeftコース]
   *  @param targetBrightness_ [カラーセンサーの目標値]
   */
  NormalCourse(Controller& controller_, bool isLeftCourse_, int targetBrightness_);

  /**
   * 左エッジ，右エッジを切り替える．
   * @param isLeftCourse_ [Leftコースである場合True]
   */
  void setIsLeftCourse(bool isLeftCourse_);

  /**
   * 実際にNormalコースを走る．
   */
  void runNormalCourse();

  /**
   * 現在のisLeftCourse（エッジ）を返すゲッター
   */
  bool getIsLeftCourse() const;

  /**
   * targetBrightness（カラーセンサの目標値）を返すゲッター
   */
  int getTargetBrightness() const;


  // HACK: CSVファイル読み込み部分は、新規にクラスを作成したほうがテストコードが書きやすい気がする
  /**
   * 渡された文字列を指定したデリミタ文字で分割して返す
   * @param input [指定したデリミタで分割したい文字列]
   * @param delimiter [指定した文字列を分割するための文字]
   * @return [指定板デリミタで分割した結果がリストで返ってくる]
   */
  std::vector<std::string> split(std::string& input, char delimiter);

  /**
   * CSVファイルのヘッダー行を渡すと、列番号に関するメンバ変数の設定と、メンバ変数(normalCoursePropertys)の初期化を行う
   * @param headerLine [CSVファイルのヘッダー（一番上）の行]
   * @return [0の時は正常終了時、それ以外の値の場合は異常終了時]
   */
  int initCsvParameters(std::string& headerLine);

  /**
   * CSVファイルのデータ行を渡すと、メンバ変数(normalCoursePropertys)の末尾にデータを追加する
   * @param parameterLine [CSVファイルのデータ（２行目以降の）行]
   * @param baseSpeed [CSVファイルの "targetSpeedCol" 列の値とこの値の和が実際の目標スピードになる]
   * @return [0の時は正常終了時、それ以外の値の場合は異常終了時]
   */
  int csvLineToNormalCourseProperty(std::string& parameterLine, int baseSpeed);

 private:
  //! コントローラー
  Controller& controller;
  //! エッジがどっちか． trueが左コース，falseが右コース
  bool isLeftCourse;
  //! カラーセンサーの目標値
  int targetBrightness;

  //! Rコース用PIDパラメータ関連ファイル名
  const std::string R_PID_FILE_NAME = "r-pid.csv";
  //! Lコース用PIDパラメータ関連ファイル名
  const std::string L_PID_FILE_NAME = "l-pid.csv";

  //! CSVファイルのヘッダー行のキー
  const char CSV_DELIMITER = ',';
  //! CSVファイルのヘッダー行のキー
  const std::string PID_FILE_HEADER_TARGET_DISTANCE = "targetDistance";
  //! CSVファイルのヘッダー行のキー
  const std::string PID_FILE_HEADER_TARGET_SPEED = "targetSpeed";
  //! CSVファイルのヘッダー行のキー
  const std::string PID_FILE_HEADER_CURVATURE = "curvature";
  //! CSVファイルのヘッダー行のキー
  const std::string PID_FILE_HEADER_TURN_PID_P = "turnP";
  //! CSVファイルのヘッダー行のキー
  const std::string PID_FILE_HEADER_TURN_PID_I = "turnI";
  //! CSVファイルのヘッダー行のキー
  const std::string PID_FILE_HEADER_TURN_PID_D = "turnD";
  //! どの列にどのパラメータが入っているかを保存する
  int targetDistanceCol = -1;
  int targetSpeedCol = -1;
  int curvatureCol = -1;
  int turnPidPCol = -1;
  int turnPidICol = -1;
  int turnPidDCol = -1;
  //! 最低限あるべき列の数（パラメータの数から決まる）
  const int MIN_COL_NUM = 6;
  //! ヘッダー行の（アイテム数）に基づくカラムの数
  int headerColNum = 0;
  //! PID関連パラメータのリスト
  std::vector<NormalCourseProperty> normalCoursePropertys;
};

#endif
