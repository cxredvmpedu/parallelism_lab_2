#set page("a4")
#set text(font: "New Computer Modern")

#let p = 3
#let A = $ vec(-9, -8, 10, 3) $
#let B = $ vec(5, 5, 7, 5) $
#let MZ = $ mat(-1, 0, -9, -4; -9, 10, 0, -8; 6, -7, 4, 1; 2, 3, 10, 3) $
#let MR = $ mat(-2, 7, 1, -10; -3, -1, -5, -8; -7, -10, 9, -8; 3, -3, -3, 0) $
#let MB = $ mat(7, 0, 7, 8; 6, 8, 1, 2; -2, 5, 4, 7; -6, 0, -4, 9) $

Вираз:
$ e = ((p dot (A dot "MB")) dot (B dot ("MZ" dot "MR")) + min(B) $

#line(length: 100%, stroke: 0.5pt)

Вхідні дані:
$
  p = #p \
  A = #A
  B = #B \
  "MZ" = #MZ
  "MR" = #MR
  "MB" = #MB
$

#line(length: 100%, stroke: 0.5pt)

Обрахунок:
#let V_0 = $ vec(-149, -14, -43, 9) $
#let V_1 = $ mat(-447, -42, -129, 27) $
$ V_0 & = "A" dot "MB" = #A dot #MB = #V_0 $
$ V_1 & = p dot V_0 = 3 dot #V_1 = #V_1 $

#let M_0 = $
  mat(
    53, 95, -70, 82;
    -36, -49, -35, 10;
    -16, 6, 74, -36;
    -74, -98, 68, -124
  )
$

$
  M_0 = "MZ" dot "MR" & = #MZ dot #MR \
                      & = #M_0
$

#let V_2 = $ vec(-397, -218, 333, -412) $
$ V_2 = B dot M_1 = #B dot #M_0 = #V_2 $

$
  a = V_1 dot V_2 & = (-447 times -397) + (-42 times -218) + (-129 times 333) + (27 times -412) \
                  & = 177459 + 9156 - 42957 - 11124 = 132534
$

$ b = min(B) = 5 $
$ e = a + b = 132534 + 5 = 132539 $
