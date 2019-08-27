# NIDAQ_simpleController
National Instrument社のデジタルアナログ変換器用アナログ出力電圧制御プログラム

NIのUSB接続のデジタルアナログ変換器をC++で制御する場合に使用できる、最もシンプルなプログラムです。

使い方
使用時のVisual Studioの設定は「DebugもしくはRelease」の「x64」とします。

コマンドは以下のようにしてあります。
* z : -delta[V]
* x : +delta[V]
* a : -10*delta[V]
* s : +10*delta[V]
* q : 終了
