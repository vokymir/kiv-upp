#import "@preview/basic-report:0.4.0": *
#import "@preview/fletcher:0.5.8" as fletcher: diagram, node, edge

#show ref: it => {
  if it.element.func() == heading {
    let loc = it.element.location()
    let page = loc.page()

    link(
    loc,
    [_#it.element.body (str. #page)_]
    )
  } else {
    it
  }
}

#show: it => basic-report(
  doc-category: "KIV/UPP 1. semestrální práce",
  heading-color: rgb("#d5a619"),
  doc-title: "Analýza vybraných open-data",
  author: "Jakub Vokoun",
  logo: image("fav_logo.svg"),
  language: "cs",
  compact-mode: false,
  it
)

= Analýza problému

Úlohu si můžeme rozdělit na podproblémy:

A) načtení dat ze souborů

B) předzpracování dat (1)

C) práci nad daty (3)

D) hledání výkyvů (2)

E) vytváření SVG map (4)

F) zápis do CSV souboru (5)

Přičemž v závorkách je uvedena souvislost s body ze zadání.

Řešení všech dílčích problémů bude snažší, pokud jsou data seřazená - stanice
podle svých identifikačních čísel uloženy do vektoru, měření podle pořadového
čísla. Nebude problém pracovat nad daty (B-F) takovým způsobem, aby se jim
neměnilo pořadí. Je tedy důležité načíst data ve správném pořadí, případně je po
načtení seřadit.

Některé podúlohy lze vykonávat paralelně, jak naznačuje graf na @fig:deps .

#figure(
  caption: "Závislost programu",
  diagram(
    node((0,0), [A], name: <A>),
    node((0,0.5), [B], name: <B>),
    node((0,1), [C], name: <C>),
    node((-1,1.5), [D], name: <D>),
    node((1,1.5), [E], name: <E>),
    node((-1,2), [F], name: <F>),

    edge(<A>, <B>),
    edge(<B>, <C>),
    edge(<C>, <D>),
    edge(<C>, <E>),
    edge(<D>, <F>)
  )
) <fig:deps>


info: vsechny inputy checknuty, vsechny jsou very fajny - tak na to budeme
spolehat

vyhral ebe... ultimate parallel commit na cachyOS
