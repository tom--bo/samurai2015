# 自たちのまとめ作っていく

*##** ゲーム情報

ターン数: 192
思考時間: 100
回復時間: 20 （ターン数）


## 方針
実験（番号）というコメントで途中経過でのパラメータを適当にいじってその時の知見を共有してみる。

- 実験1
 - hidieMerits　-0.1にしたのは失敗だった。
 - 相変わらず塗りつぶし合いが止まらない。
 - selfTerritoryとterritoryを上手く設定すれば回避できそう


- 実験2
 - greedyPlayer0,1,2でselfTerritoryMeritsを-0.3, -0.1, 0.1と変化させてみた
 - 味方同士の塗り合いがだいぶ防げる用になった。
 - id: 1,4が周囲に自分と味方のマスしかないときに全く動か亡くなった。
 - 敵のマスのある方向に移動するというMeritsを計算に入れる必要がありそう。
 - id: 0,3は武器が槍だから動かないという状況は起こりにくかったっぽい
 - 23turn目、赤が水色の3マスを塗った方が良いはずなのに塗に行かない。結果的に殺されなくて住んでいるけど、ここもバグがありそう


## Observation
###  02/19 24時の1ページ目からomuAI入っているの

- 剣
 - http://arena.ai-comp.net/contests/1/battle_results/19268
 - http://arena.ai-comp.net/contests/1/battle_results/19250
 - http://arena.ai-comp.net/contests/1/battle_results/19314
 - 割りとすんなり真ん中行って敵陣まで塗ってる


- 斧
 - http://arena.ai-comp.net/contests/1/battle_results/19241


- 槍
 - http://arena.ai-comp.net/contests/1/battle_results/19228
 - http://arena.ai-comp.net/contests/1/battle_results/19217
 - http://arena.ai-comp.net/contests/1/battle_results/19213
 - http://arena.ai-comp.net/contests/1/battle_results/19212
 - http://arena.ai-comp.net/contests/1/battle_results/19100
 - 相手の斧が入り込んで来ないようにはしてそう
 - 味方が斧を防ぐ位置にいれば任せてる感じもする
 - ↑そうでもないとにかくぬってるだけか。。。
 - 味方のいないところに行ってる感時が結構ある


## Zoukin
- http://arena.ai-comp.net/contests/1/battle_results/19311
 - 槍：真ん中ウロウロがとにかくやばい
 -  殺すのとアサシンもいいけど塗る特典上げないとまずそう

- http://arena.ai-comp.net/contests/1/battle_results/19308
 - 剣：アサシンしたり殺すことに固執しすぎ

- http://arena.ai-comp.net/contests/1/battle_results/19302
 - 槍：まあまあいい感じ
 - 味方から離れるはあってもいいかも
 
- http://arena.ai-comp.net/contests/1/battle_results/19297
 - 剣：真ん中以降に行けてなさすぎ
 - 空白塗るメリット上げたほうが良さそう
- 
- http://arena.ai-comp.net/contests/1/battle_results/19289
 - 剣：結構ヤバイ感じある
 - 運が悪いのでなければ敗因として重要そう
 
- http://arena.ai-comp.net/contests/1/battle_results/19288
 - 槍：序盤に自分の周辺塗ったほうが良かったのでは？
 - 味方から離れるは必須な気がする
 
- http://arena.ai-comp.net/contests/1/battle_results/19287
 - 味方からたまたま離れてたから良い感じになった気がする
 
 
- http://arena.ai-comp.net/contests/1/battle_results/19286
 - 剣：対同じ武器に対して弱いかも
 
 
- http://arena.ai-comp.net/contests/1/battle_results/19285
 - 剣：厳しい・味方だろうか？
 
 
- http://arena.ai-comp.net/contests/1/battle_results/19283
 - 斧：周辺が剣ならもう少し様子見たほうが良さそう
 - respawnした後の行動に注意
 
 
- http://arena.ai-comp.net/contests/1/battle_results/19281
 - 斧：respawn後にはめられすぎ
 
- http://arena.ai-comp.net/contests/1/battle_results/19280
 - 槍：わからん味方か？
 
- http://arena.ai-comp.net/contests/1/battle_results/19279
 - 斧：respawn後にはめられすぎ
 
- http://arena.ai-comp.net/contests/1/battle_results/19278
- http://arena.ai-comp.net/contests/1/battle_results/19276
- http://arena.ai-comp.net/contests/1/battle_results/19275
 


