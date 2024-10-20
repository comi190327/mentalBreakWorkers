= 使用しているツール

//lead{
ここからは、おまけとして私が普段使用しているツールを紹介します。
//}

== Todoist
TODOリストツールです。非常に忘れっぽいのでプライベート用に使用しています。@<br>{}
画像のようにプロジェクトを分け（@<img>{todoist_project}）、その中でさらにセクションを追加できる（@<img>{todoist_section}）のでお気に入りです。無料版で予定日まで設定できるのですが、有料版にするとリマインダーや位置情報を付加することもできます。@<br>{}
Webでも利用できますし、スマホアプリもあります。ウェアラブル端末と紐づけることもできます。Chromeの拡張機能にもあるそうです（私は使ったことないです）。


 * Web：@<href>{https://todoist.com/ja}@<br>{}
 * iOS：@<href>{https://apps.apple.com/jp/app/id572688855}@<br>{}
 * AndroidOS：@<href>{https://play.google.com/store/apps/details?id=com.todoist&hl=ja}@<br>{}


//image[todoist_project][プロジェクトを分けた図][scale=0.5]{
//}

//image[todoist_section][セクションを分けた図]{
//}

== Focus To-Do
ポモドーロタイマーアプリです（@<img>{focus_todo_timer}）。ポモドーロタイマーの時間を調節できます。また、タスクとポモドーロタイマーの個数を紐づけることができます（@<img>{focus_todo_setting}）。@<br>{}
また、ホワイトノイズを流すこともできます（@<img>{focus_todo_whitenoise}）。私は無音状態や音楽より、ホワイトノイズを流すほうが集中できるタイプなので、在宅勤務のときに重宝しています。@<br>{}
スマホアプリ＆ウェアラブル端末で使用していたのですが、執筆用に調べたら、なんとChrome拡張機能でも存在しました。@<br>{}
有料版を購入すれば全デバイスでデータを共有できます。また、作業時間の記録もでき、分析レポートも確認することができます。

 * Web：@<href>{https://www.focustodo.cn/}@<br>{}
 * iOS：@<href>{https://apps.apple.com/jp/app/id966057213}@<br>{}
 * AndroidOS：@<br>{}@<href>{https://play.google.com/store/apps/details?id=com.superelement.pomodoro&hl=ja}@<br>{}

//image[focus_todo_timer][タイマー画面][scale=0.5]{
//}

//image[focus_todo_setting][タイマー時間の設定画面][scale=0.5]{
//}

//image[focus_todo_whitenoise][ホワイトノイズの設定画面][scale=0.5]{
//}

== Google Spreadsheets＆LINE messaging API
これを読んでいる方で知らない方はいないであろうGoogle SpreadsheetsとLINEを連携することで、勤務表作成を（ある程度）自動化しています。@<br>{}
LINEの特定のチャンネルで始業時間・終業時間・休憩時間を入れることで（@<img>{line_talk}）、スプレッドシートの当日のセルに勤務時間を自動入力するようにしています。（@<img>{spreadsheets_cell}）@<br>{}

//image[line_talk][LINEトーク画面]{
//}

//image[spreadsheets_cell][スプレッドシート画面][scale=0.5]{
//}


連携する際はGoogle App ScriptとLINE messaging APIを使用しています。@<br>{}
実際に使用しているコードを記載します。もし半自動で勤務表を作成したい方がいれば参考にしてみてください！

//cmd{

// LINE developerで登録したチャネルアクセストークン
var ACCESS_TOKEN = 'LINE developerで登録したチャネルアクセストークン';
// LINEへ応答メッセージを送るAPI（LINE messaging API）
var LINE_ENDPOINT = "https://api.line.me/v2/bot/message/reply"

//スプレッドシートのID
let SHEET_ID = "スプレッドシートのID";

// LINEからPOSTリクエストが渡されてきたときに実行される処理
function doPost(e) {
 console.log(e);
  // LINEからPOSTされるJSON形式のデータをGASで扱える形式(JSオブジェクト)に変換
  var json = JSON.parse(e.postData.contents);
  console.log(json);

  // LINE側へ応答するためのトークンを作成(LINEからのリクエストに入っているので、それを取得する)
  var reply_token = json.events[0].replyToken;
  console.log(reply_token);
  if (typeof reply_token === 'undefined') {
    return;
  }

  // LINEから送られてきたメッセージを取得
  var user_message = json.events[0].message.text;
  console.log(user_message);
  // 改行で区切って配列にする
  var user_msgarray = user_message.split(/\r\n|\n/);
  console.log(user_msgarray);

  // 応答メッセージ本文
  var resultMsg = registData(user_msgarray);


  // バリデーション
  /* 
  var validate = isValid(user_msgarray);
  if (validate != "OK") {
    resultMsg = validate;
  } else if (validate == "OK") {
    resultMsg = registData(user_msgarray);
  }

  */

  // 応答用のメッセージを作成
  var message = {
                  "replyToken"  : reply_token,
                  "messages"    : [{"type": "text",                 // メッセージのタイプ(画像、テキストなど)
                                    "text" : resultMsg}] // メッセージの内容
                };

  // LINE側へデータを返す際に必要となる情報
  var options = {
    "method" : "post",
    "headers" : {
      "Content-Type" : "application/json; charset=UTF-8",  // JSON形式を指定、LINEの文字コードはUTF-8
      "Authorization" : "Bearer " + ACCESS_TOKEN           // 認証タイプはBearer(トークン利用)、アクセストークン
    },
    "payload" : JSON.stringify(message)                    // 応答文のメッセージをJSON形式に変換する
  };

  // LINEへ応答メッセージを返す
  UrlFetchApp.fetch(LINE_ENDPOINT, options);
}

// スプレッドシートへの書き込み
function registData(user_msgarray) {
  var msg = "";
  
  // 対象のスプレッドシートを取得
  var spreadSheet = SpreadsheetApp.openById(SHEET_ID);

  // 本日の日付を取得
  // var today = Utilities.formatDate(new Date(), "Asia/Tokyo", "yyyy/MM/dd");
  var todayDay = Utilities.formatDate(new Date(), "Asia/Tokyo", "d");
  console.log(todayDay);

  // 書き込む対象のシートを取得
  var sheetName = Utilities.formatDate(new Date(), "Asia/Tokyo", "yyyy/MM");
  var targetSheet = spreadSheet.getSheetByName(sheetName);
  console.log(sheetName);
  console.log(targetSheet);

  // 各時間を変数に入れる
  var startTime = user_msgarray[0];
  var endTime = user_msgarray[1];
  var breakTime = user_msgarray[2];
  console.log(startTime);
  console.log(endTime);
  console.log(breakTime);

  if (targetSheet != null) {
    // 今日日付の行を取得
    todayRow = Number(todayDay) + 1;
    console.log(todayRow);

    // 書き込み
    targetSheet.getRange(todayRow, 2).setValue(startTime); // 開始時刻
    targetSheet.getRange(todayRow, 3).setValue(endTime); // 終了時刻
    targetSheet.getRange(todayRow, 4).setValue(breakTime); // 休憩時間
    msg = "以下の情報で書き込みしました。\n" +
          "開始時刻：" +  startTime + "\n" +
          "終了時刻：" +  endTime + "\n" +
          "休憩時間：" +  breakTime;
  } else {
    // 対象のシートが見つからない場合
    msg = "シート名：" + sheetName + "\nが見つかりません";
  }
  console.log(msg);
  return msg;

}

//}