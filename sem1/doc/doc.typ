#import "@preview/basic-report:0.4.0": *
#import "@preview/fletcher:0.5.8" as fletcher: diagram, node, edge

#set math.equation(numbering: "1.")

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

= Zadání

Zkopírováno z portálu CoureWare.

== Popis dat

Z portletu se soubory si stahněte soubor "SP1 - Průměrné teploty" - ten bude
Vaší výchozí datovou sadou. Pro začátek testujte práci s malou sadou, ta
obsahuje původní data. Datové sady označené jako "střední" a "velké" byly
rozšířeny o stanice náhodně vygenerované.

Původní data jsou extrahována z měření všech meteorologických stanic v ČR. Zdroj
dat je
#link("https://www.chmi.cz/historicka-data/pocasi/denni-data/Denni-data-dle-z.-123-1998-Sb")[ČHMÚ],
data jsou volně použitelná dle zákona 123/1998 Sb. Data byla předzpracována a
jsou poskytnuta studentům dle
#link("https://www.chmi.cz/files/portal/docs/meteo/ok/open_data_2023/Podminky_uziti_udaju.pdf")[oficiálních
  pokynů]. Datové sady větší jsou uměle rozšířeny o náhodné stanice, které
přejímají měření nejbližší stanice s vygenerovanou malou odchylkou.

Soubor `stanice.csv` obsahuje seznam měřicích stanic. Jde o CSV soubor s
hlavičkou a sloupci oddělenými středníkem, kódování je UTF-8. Obsahuje tyto
položky:

- `id` - identifikátor stanice, podle toho budete přiřazovat data z druhého
  souboru s daty
- `name` - orientační název stanice
- `latitude`, `longitude` - zeměpisná šířka a délka umístění stanice

Soubor `mereni.csv` obsahuje všechna agregovaná měření z těchto stanic. Každý
řádek představuje jednu průměrnou denní teplotu z jedné stanice. Jde rovněž o
CSV soubor s hlavičkou a sloupci oddělenými středníkem. Obsahuje tyto položky:

- `station_id` - ID stanice ze souboru stanice.csv
- `ordinal` - pořadové číslo měření (doplňkové)
- `year`, `month`, `day` - rok, měsíc a den hodnoty
- `value` - hodnota

Pokud se tedy například objeví řádek `24;125;1994;2;12;3.5`, lze usuzovat, že
dle stanice 24 byla 12. února 1994 průměrná teplota v místě měření 3.5 °C (a že
to byla 125. hodnota, kterou stanice naměřila).

== Zadání práce

Vaším úkolem bude provést několik transformací a analýzu dat:

+ předzpracujte data
  - odstraňte stanice, které mají naměřeno méně, než 5 let souvislých hodnot (tj.
   pokud se v datech objevuje kdekoliv alespoň 5 souvislých let, stanici ponechte)
  - odstraňte stanice, které za naměřené období reportovaly v průměru méně, než
   100 hodnot ročně
+ identifikujte meziročný výkyvy
  - meziroční výkyv je detekován, pokud se průměrná teplota daný měsíc ve dvou po
    sobě jdoucích letech liší o více, jak 75 % rozdílu mezi minimem a maximem
  - např.: pokud v datech objevíte, že nejvyšší a nejnižší průměrná červnová 
    teplota je 12 a 19 stupňů (tj. ze všech průměrů za všechny roky), pak je
    hledaná hranice rozdílu 0.75\*(19-12) = 5.25 stupně 
    - pak když zjistíte, že se průměrná teplota v červnu 2000 a v červnu 2001
      liší o 6 stupňů (> 5.25), detekujete výkyv 
    - pokud se ale průměrná teplota v červnu 2001 a červnu 2002 liší o 2 stupně,
      je všechno v pořádku 
+ stanovte průměrné teploty ve všech měřicích místech v každém měsíci
+ vykreslete mapu ČR ve formátu SVG pro každý měsíc (leden.svg, ...,
  prosinec.svg), kde vykreslíte všechny stanice jako body, které obarvíte na škále
  od modré přes žlutou (střed rozsahu) až k červené (lineárně interpolujte) 
  - jako minimum škály berte globální minimum teplot ze všech naměřených dat 
  - analogicky maximum škály 
  - tj. pokud zjistíte, že ze všech dat bylo nejnižší číslo -20, tam bude
    začínat modrá barva; pokud bude maximem 40, tam bude končit barva červená
  - slepá mapa ČR je k dispozici taktéž v portletu se soubory jako "Slepá mapa 
    ČR (SVG)" 
    - jak ji použijete je na Vás - zda ji dáte do kódu jako zdroj a doplníte
      body nebo ji necháte jako soubor přiloženou, popř. jakákoliv jiná kombinace;
      důležitý je výstup, ale vyvarujte se pochopitelně očividným nečistým řešením
    - v této mapě levý horní roh odpovídá souřadnicím 51.03806105663445,
      12.102209054269062 a pravý spodní roh souřadnicím 48.521003814763994,
      18.866923511078615 
    - pro vykreslení zanedbejte zakřivení země a umístěte body s lineárně
      interpolovanými souřadnicemi, jako kdyby byla Země placatá
  - nemusíte implementovat SVG "v plné kráse" - stačí se naučit, jak vykreslit 
    vyplněné kolečko, které snadno naformátujete jako text
+ do souboru vykyvy.csv vypište identifikované meziroční výkyvy
  - formátem je standardní CSV se středníkem jako oddělovačem 
  - zapište hlavičku CSV, budete ukládat následující sloupce: 
    - ID stanice
    - měsíc detekce (jeho číslo, tj. 1 = leden, ...) 
    - druhý z roků detekce (tj. pokud jste detekovali výkyv mezi roky 2000 a 
      2001, zapište 2001) 
    - hodnota rozdílu teplot 
+ zdokumentujte Vaše uvažování 
  - vytvořte průvodní dokument, ve kterém popíšete, jak jste nad problémem 
    uvažovali
  - proveďte důkladnou analýzu problému, identifikujte vhodná místa pro 
    paralelizaci a následně zhodnoťte implementaci
    - určitě uveďte popis paralelní úlohy (precedenční graf, kritická cesta,
      vydedukované teoretické urychlení z toho) 
  - spočítejte metriky relevantní k paralelizaci 
    - urychlení 
    - efektivita paralelizace 
    - Amdahlův zákon - porovnejte s očekáváním, zdůvodněte výsledky 
    - Gustafsonova metrika - porovnejte s očekáváním, zdůvodněte výsledky 
  - uveďte, na jakém PC jste k výsledkům došli (CPU, RAM)

Paralelizujte všechny kroky v nejvyšší možné míře, která by se mohla vyplatit. Svá rozhodnutí dobře zdůvodněte.

Program bude spouštěn z příkazové řádky, kdy prvním parametrem je vždy cesta k
souboru se stanicemi, druhým parametrem vždy cesta k souboru s odpovídajícím
měřením a třetím parametrem je vždy přepínač sériové/paralelní verze. *Řešení
tedy obsahuje obě verze - jak sériovou, tak paralelní!* Příklad:

`./upp_sp1 stanice.csv mereni.csv --serial`

`./upp_sp1 C:\Data\stanice.csv ..\mereni.csv --parallel`

= Analýza problému

V této kapitole je primárně rozebírán problem paralelizace, tedy ne jak
implementovat samotné řešení, ale jak jej paralelizovat a optimalizovat.
Na úvod této kapitoly je ale nezbytné zmínit některé důležité skutečnosti.

- Řešení všech dílčích problémů bude snažší, pokud jsou data seřazená - stanice
  podle svých identifikačních čísel uloženy do vektoru, měření podle pořadového
  čísla. Nebude problém pracovat nad daty takovým způsobem, aby se jim neměnilo
  pořadí. Je tedy důležité načíst data ve správném pořadí, případně je po
  načtení seřadit.
  - #sym.arrow Zaveden předpoklad, že načítaná data jsou pěkná - správně
    seřazená. Tento předpoklad byl experimentálně ověřen pro danou množinu dat.
    Pro jakákoliv další data by nemusel platit.

== Paralelizace úloh

Úlohu si můžeme rozdělit na podproblémy:

#set enum(numbering: "A")

+ načtení dat ze souborů
+ předzpracování dat (1)
+ práci nad daty (3)
+ hledání výkyvů (2)
+ vytváření SVG map (4)
+ zápis do CSV souboru (5)

Přičemž v závorkách je uvedena souvislost s body ze zadání.

#figure(
  caption: "Precedenční graf úlohy.",
  diagram(
    node((0,0), [A: čtení], name: <A>),
    node((0,1), [B: filtrace], name: <B>),
    node((0,2), [C: výpočty], name: <C>),
    node((-1,3), [D: identifikace výkyvů], name: <D>),
    node((1,3), [E: export SVG map], name: <E>),
    node((-1,4), [F: zápis výkyvů do CSV], name: <F>),

    edge(<A>, <B>, "->"),
    edge(<B>, <C>, "->"),
    edge(<C>, <D>, "->"),
    edge(<C>, <E>, "->"),
    edge(<D>, <F>, "->")
  )
) <fig:precedence>

Precedenci úloh pozorujeme na grafu @fig:precedence. Graf téměř přesně kopíruje
úlohy v zadání, ale vznikl na základě analýzy vstupů a výstupů jednotlivých úloh
(zaznamenané v @tab:tasks-in-out).

#figure(
table(
  columns: (auto, auto, auto),
  align: horizon+center,
  [*Úloha*], [*Vstup*], [*Výstup*],

  [A - Načtení dat], [cesty ke vstupním souborům], [vektor stanic, každá 
    obsahuje svůj vektor měření],

  [B - Filtrace], [vektor všech stanic], [vektor obsahující pouze platné stanice],

  [C - Statistiky], [vektor stanic], [vektor stanic, každá obsahuje spočtené
    statistiky],

  [D - Výkyvy], [vektor stanic obsahující statistiky], [vektor stanic,
    každá obsahuje svůj vektor detekovaných výkyvů],

  [E - SVG mapy], [vektor stanic obsahující statistiky], [12 obrázků, každý pro
    jeden měsíc],

  [F - zápis CSV], [vektor stanic obsahující detekované výkyvy], [1 soubor se
    všemi detekovanými výkyvy],
),
  caption: [Seznam identifikovaných úloh včetně jejich vstupů a výstupů.]
)<tab:tasks-in-out>

Na základě znalostí uvedených v @fig:precedence a @tab:tasks-in-out očekáváme,
že úloha je alespoň částečně paralelizovatelná na úrovni podúloh, a to ve fázi
následující po podproblému C (výpočet statistik).

=== Kritická cesta

Pro nalezení kritické cesty použijeme standardní algoritmus, jehož provedení je
znázorněno v @fig:critical-path-serial (do grafu byly uměle přidány body startu
a konce běhu programu). Pro hledání byly použity zprůměrované
časy jednotlivých úloh v sériovém řešení (může se lišit od časů v paralelní
verzi).

#figure(
  caption: "Kritická cesta podle časů získaných pomocí sériové verze, časy v
  milisekundách. Kritická cesta je vyznačená červenou barvou.",
  diagram(
    node((0, -1), [START], name: <START>),
    node((0,0), [A: čtení #linebreak()
      #box(fill:red.lighten(60%), inset: 2pt)[0 | 0]], name: <A>),
    node((0,1), [B: filtrace #linebreak()
      #box(fill:red.lighten(60%), inset: 2pt)[6 000 | 6 000]], name: <B>),
    node((0,2), [C: výpočty #linebreak()
      #box(fill:red.lighten(60%), inset: 2pt)[6 110 | 6 110]], name: <C>),
    node((-1,3), [D: identifikace výkyvů #linebreak()
      #box(fill:blue.lighten(60%), inset: 2pt)[6 218 | 6 225]], name: <D>),
    node((1,3), [E: export SVG map #linebreak()
      #box(fill:red.lighten(60%), inset: 2pt)[6 218 | 6 218]], name: <E>),
    node((-1,4), [F: zápis výkyvů do CSV #linebreak()
      #box(fill:blue.lighten(60%), inset: 2pt)[6 221 | 6 228]], name: <F>),
    node((0,5), [END #linebreak()
      #box(fill:red.lighten(60%), inset: 2pt)[6 243 | 6 243]], name: <END>),

    edge(<START>, <A>, "->"),
    edge(<A>, <B>, "->", stroke: red, label: "6 000"),
    edge(<B>, <C>, "->", stroke: red, label: "110"),
    edge(<C>, <D>, "->", stroke: blue, label: "108"),
    edge(<C>, <E>, "->", stroke: red, label: "108"),
    edge(<D>, <F>, "->", stroke: blue, label: "3"),
    edge(<F>, <END>, "->", stroke: blue, label: "15"),
    edge(<E>, <END>, "->", stroke: red, label: "25"),
  )
) <fig:critical-path-serial>


== Datový paralelismus

Všechny úlohy (A-F) lze paralelizovat, například pomocí cyklů. Následuje
analýza, jakým způsobem paralelizovat konkrétní úlohy spolu s původním sériovým
řešením v @tab:tasks-comparison.

#figure(
table(
  columns: (auto, auto, auto),
  align: horizon+center,
  [*Úloha*], [*Sériová verze*], [*Paralelní verze*],

  [A - Načtení dat], [Načtení celého souboru do bufferu, následné procházení
    bufferu a načítání do struktur.], [Po načtení bufferu jej rozdělit podle
    počtu jader, každé načítá část.],

  [B - Filtrace], [Sekvenční kontrola všech stanic pomocí nezávislých funkcí.],
    [Využití trapného paralelismu (funkcí standardní knihovny), rozdělení stanic
    mezi jádra.],

  [C - Statistiky], [Výpočty průměrů, minimálních a maximálních hodnot jsou na
    sobě mezi stanicemi nezávislé, vykonávají se sekvenčně.], [Trapný
    paralelismus, rozdělení stanic mezi jádra.],

  [D - Výkyvy], [Detekce výkyvů na sobě mezi stanicemi nezávislá, vykonává se
    sekvenčně.], [Trapný paralelismus, rozdělení stanic mezi jádra.],

  [E - SVG mapy], [Cyklus pro všechny měsíce, každý zakreslí všechny stanice.], 
    [Trapný paralelismus, rozdělení měsíců mezi jádra. Nedojde ke konfliktu,
    protože každé jádro píše do jednoho souboru a ze stanic pouze čte.],

  [F - zápis CSV], [Prochází všechny stanice a zapisuje jejich výkyvy do
    souboru.], [Rozdělení stanic mezi jádra, připravují řetězec k zapsání, které
    proběhne poté na jednom jádře.],
),
  caption: [Shrnutí fungování sériové verze programu a navržení paralelní.]
)<tab:tasks-comparison>

== Teoretické urychlení

Předpokládejme, podle informací v @tab:tasks-comparison, že všechny úlohy mohou
být paralelizované.

=== Amdahlův zákon

Amdahlovův zákon definuje urychlení jako:

$
S_#text("lat") = 1 / ((1 - p) + p / s)
$ <eq:amdhal>

kde $p$ je podíl čas běhu paralizovatelných úloh vůči všem úlohám
a $s$ je maximální možné urychlení.

#figure(
table(
  columns: (auto, auto),
  align: horizon+center,
  [*Úloha*], [*Čas [ms] (sériová verze)*],

  [A - Načtení dat], [6 000], 

  [B - Filtrace], [110],

  [C - Statistiky], [108],

  [D - Výkyvy], [3], 

  [E - SVG mapy], [25], 

  [F - zápis CSV], [15], 

  [*Celkem*], [*6 261*]
),
  caption: [Průměrné doby vykonávání jednotlivých úloh sériovým způsobem.]
)<tab:tasks-timed-serial>


Podle předpokladu, že všechny úlohy jsou paralelizovatelné, z
@tab:tasks-timed-serial vidíme, že $p = (sum_A^F (t)) / 24905 = 1$ a řekneme, že $s
= #text("#jader") = 8$. Po dosazení do rovnice dostáváme:

$
S_#text("lat") = 1 / ((1 - 1) + 1 / 8) = 8
$ <eq:amdhal-solved>

Z @eq:amdhal-solved vidíme, že teoretické maximální zrychlení je 800%. To
odpovídá naivnímu předpokladu, že se všechny úlohy podaří rozdělit mezi osm
jader.

=== Gustafsonův zákon

Definován jako:

$
S = (1 - a)P + a
$ <eq:gustafson>

kde $a$ je podíl sériové části kódu a
$P$ je počet procesorových jader.

Pro náš předpoklad, že je celý program paralelizovatelný, dostáváme $a = 0$.

$
S = (1 - 0) 8 + 0 = 8
$ <eq:gustafson-solved>

= Návrh a implementace paralelizace

= Měření a zhodnocení

#figure(
table(
  columns: (auto, auto, auto),
  align: horizon+center,
  [*Úloha*], [*Čas [ms] (sériová verze)*], [*Čas [ms] (paralelní verze)*],

  [A - Načtení dat], [6 000], [3 500],

  [B - Filtrace], [110], [69],

  [C - Statistiky], [108], [33],

  [D - Výkyvy], [3], [1],

  [E - SVG mapy], [25], [18],

  [F - zápis CSV], [15], [4],

  [*Celkem*], [*6 261*], [*3 625*],
),
  caption: [Průměrné doby vykonávání jednotlivých úloh.]
)<tab:tasks-timed>

== Konfigurace testovacího prostředí

Referenční měření probíhalo na stroji s následující konfigurací:

- *CPU:* Intel(R) Core(TM) i5-1035G1 CPU @ 1.00GHz, 4 fyzická / 8 logických
  jader
- *RAM:* 8 GB DDR4
- *OS:* CachyOS (Linux)
- *Překladač:* GCC 15.2.1
  - Release mode (`-O3`)

== Naměřené časy

Pro měření byla použita velká datová sada (`velke_mereni.csv`).

== Metriky paralelizace

Na základě celkových časů můžeme vypočítat základní metriky s využitím $p = 8$ vláken.

*Dosažené urychlení (Speedup):*
$ S = T_s / T_p =  /  approx  $

*Efektivita (Efficiency):*
$ E = S / p =  / 8 approx  quad ( %) $

*Amdahlův zákon:*

*Gustafsonův zákon (škálované urychlení):*

= Závěr
