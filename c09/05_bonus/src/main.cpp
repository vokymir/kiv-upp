#include <iostream>

#include "controller.h"
#include "fir_filter.h"
#include "patient.h"
#include "sensor.h"
#include <mpi.h>

enum Rank { PATIENT = 0, SENSOR = 1, FILTER = 2, CONTROLLER = 3 };

enum Tag {
  TAG_GLUCOSE = 100,
  TAG_NOISY_GLUCOSE = 101,
  TAG_FILTERED_GLUCOSE = 102,
  TAG_DOSE = 103
};

void worker_patient(size_t iter) {
  CVirtual_Patient patient;

  for (size_t i = 0; i < iter; ++i) {
    patient.Step();

    double current_glucose = patient.Get_Current_Glucose();
    std::cout << "Aktualni glykemie: " << current_glucose << std::endl;

    MPI_Send(&current_glucose, 1, MPI_DOUBLE, Rank::SENSOR, TAG_GLUCOSE,
             MPI_COMM_WORLD);

    double dose = 0.0;
    MPI_Recv(&dose, 1, MPI_DOUBLE, Rank::CONTROLLER, TAG_DOSE, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);

    if (dose > 0.0) {
      patient.Dose_Insulin(dose);
    }
  }
}

void worker_sensor(size_t iter) {
  CVirtual_Sensor sensor;

  for (size_t i = 0; i < iter; ++i) {
    double glucose = 0.0;
    MPI_Recv(&glucose, 1, MPI_DOUBLE, Rank::PATIENT, TAG_GLUCOSE,
             MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    double noisy_glucose = sensor.Get_Glucose_Reading(glucose);

    MPI_Send(&noisy_glucose, 1, MPI_DOUBLE, Rank::FILTER, TAG_NOISY_GLUCOSE,
             MPI_COMM_WORLD);
  }
}

void worker_filter(size_t iter) {
  CFIR_Filter filter;

  for (size_t i = 0; i < iter; ++i) {
    double noisy_glucose = 0.0;
    MPI_Recv(&noisy_glucose, 1, MPI_DOUBLE, Rank::SENSOR, TAG_NOISY_GLUCOSE,
             MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    filter.Add_Sample(noisy_glucose);
    double filtered_glucose = filter.Get_Average();

    MPI_Rsend(&filtered_glucose, 1, MPI_DOUBLE, Rank::CONTROLLER,
              TAG_FILTERED_GLUCOSE, MPI_COMM_WORLD);
  }
}

void worker_controller(size_t iter) {
  CVirtual_Controller controller;

  for (size_t i = 0; i < iter; ++i) {
    double filtered_glucose = 0.0;
    MPI_Recv(&filtered_glucose, 1, MPI_DOUBLE, Rank::FILTER,
             TAG_FILTERED_GLUCOSE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    double dose = controller.Get_Control_Response(filtered_glucose);

    MPI_Ssend(&dose, 1, MPI_DOUBLE, Rank::PATIENT, TAG_DOSE, MPI_COMM_WORLD);
  }
}

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);

  int rank = 0, size = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (size != 4) {
    if (rank == 0) {
      std::cerr << "Tento program vyzaduje presne 4 MPI procesy.\n";
    }
    MPI_Finalize();
    return 1;
  }

  size_t iter = 1000;
  if (rank == PATIENT) {
    worker_patient(iter);
  } else if (rank == SENSOR) {
    worker_sensor(iter);
  } else if (rank == FILTER) {
    worker_filter(iter);
  } else if (rank == CONTROLLER) {
    worker_controller(iter);
  }

  MPI_Finalize();
  return 0;
}

// int main(int argc, char *argv[]) {
//
//   /*
//    * Velmi jednoducha simulace pacienta s diabetem 1. typu
//    *
//    * CVirtual_Patient je model pacienta - umime do nej davkovat inzulin a
//    jidlo
//    *                  - model si sam vymysli, kdy se naji (nahodne)
//    *                  - inzulin se ale musi davat externe
//    *                  - model jde krokovat, abychom mohli simulovat plynuti
//    casu
//    *                  - z modelu si vzdy muzeme vyzvednout aktualni hladinu
//    * glukozy
//    *
//    * CVirtual_Sensor je model senzoru - umime z nej vyzvednout aktualni
//    zmerenou
//    * hladinu glukozy
//    *                  - fakticky je to jenom kopie hodnoty z pacienta
//    obohacena
//    * o sum
//    *                  - toto simuluje realny scenar, kdy senzor vzdy vnasi do
//    * mereni chybu
//    *
//    * CFIR_Filter je FIR (Finite Impulse Response) filtr - v podstate jde o
//    * prumer poslednich N hodnot
//    *                  - toto je zase komponenta, ktera se obvykle objevi v
//    * realnych situacich na elementarni odstraneni sumu
//    *                  - samozrejme existuji lepsi metody odstraneni sumu
//    *
//    * CVirtual_Controller je model ridiciho algoritmu - na zaklade prectene a
//    * prefiltrovane hodnoty urci, kolik se ma dat inzulinu
//    *                  - sam o sobe ma interne casovac, ktery nedovoli
//    davkovat
//    * vzdy
//    *                  - vraci tedy bud 0 (tzn. nechceme davkovat nic) nebo
//    cislo
//    * vetsi nez 0 znamenajici kolik jednotek inzulinu se ma podat
//    *                  - vystup se musi predat "do pacienta", tj. tvorime
//    vlastne
//    * neco jako uzavrenou smycku (z teorie rizeni - closed loop, feedback
//    loop)
//    */
//
//   CVirtual_Patient patient;
//   CVirtual_Sensor sensor;
//   CFIR_Filter filter;
//   CVirtual_Controller controller;
//
//   for (size_t i = 0; i < 1000; i++) {
//     patient.Step();
//
//     std::cout << "Aktualni glykemie: " << patient.Get_Current_Glucose()
//               << std::endl;
//
//     filter.Add_Sample(
//         sensor.Get_Glucose_Reading(patient.Get_Current_Glucose()));
//     const double filtered_glucose = filter.Get_Average();
//
//     double dose = controller.Get_Control_Response(filtered_glucose);
//     if (dose > 0) {
//       patient.Dose_Insulin(dose);
//     }
//   }
//
//   /*
//    * Vas bonusovy ukol:
//    *   - pouzijte implementace vyse, ale s tim, ze kazda komponenta pobezi na
//    * jinem uzlu v MPI clusteru
//    *   - kazdy MPI uzel bude obstaravat jednu entitu a bude komunikovat s
//    * nasledujicim uzlem
//    *   - posledni uzel (ridici algoritmus) bude komunikovat s uzlem prvnim
//    * (pacient)
//    *     -> vytvarite tedy kruhovou virtualni topologii
//    *
//    *   - pravidla pro komunikaci:
//    *     - pacient + senzor = standardni rezim komunikace (buffery, bez
//    * explicitni a vynucene synchronizace, tj. MPI_Send a _Recv)
//    *     - senzor + FIR filter = standardni
//    *     - FIR filter + ridici algoritmus = "ready" rezim, tj. ridici
//    algoritmus
//    * musi uz cekat v MPI_Recv, kdyz FIR filter odesila data (najdete
//    prislusnou
//    * variantu Send funkce!)
//    *                                      - pokud tam ridici algoritmus uz
//    * neceka, tak se data zahodi
//    *     - ridici algoritmus + pacient = synchronni rezim, tj. ridici
//    algoritmus
//    * ceka, nez si pacient vyzvedne hodnotu (najdete prislusnou variantu Send
//    * funkce!)
//    */
//
//   return 0;
// }
