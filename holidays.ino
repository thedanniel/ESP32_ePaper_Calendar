void feriado(int mes) {
  switch (mes) {
    case 1:
      display.print("1 - Confraternização Universal");
      break;
    case 2:
      if (MesCarnaval(dataAtual.ano) == 2) {
        display.print(String(DiaCarnaval(dataAtual.ano) - 1) + " e " + String(DiaCarnaval(dataAtual.ano)) + " - Carnaval\n");
        display.print(String(DiaCarnaval(dataAtual.ano) + 1) + " - Quarta-feira de Cinzas");
      }
      break;
    case 3:
      if (MesPascoa(dataAtual.ano) == 3) {
        display.print(String(DiaPascoa(dataAtual.ano) - 2) + " - Paixão de Cristo\n");
      } else if (MesCarnaval(dataAtual.ano) == 3) {
        display.print(String(DiaCarnaval(dataAtual.ano) - 1) + " e " + String(DiaCarnaval(dataAtual.ano)) + " - Carnaval\n");
        display.print(String(DiaCarnaval(dataAtual.ano) + 1) + " - Quarta-feira de Cinzas\n");
      }
      break;
    case 4:
      if (MesPascoa(dataAtual.ano) == 4) {
        if (DiaPascoa(dataAtual.ano) <= 21) {
          display.print(String(DiaPascoa(dataAtual.ano) - 2) + " - Paixão de Cristo\n");
          display.print(String(DiaPascoa(dataAtual.ano)) + " - Páscoa\n");
          display.print("21 - Tiradentes");
        } else {
          display.print("21 - Tiradentes\n");
          display.print(String(DiaPascoa(dataAtual.ano) - 2) + " - Paixão de Cristo\n");
          display.print(String(DiaPascoa(dataAtual.ano)) + " - Páscoa");
        }
      }
      break;
    case 5:
      display.print("1 - Dia do Trabalhador\n");
      if (MesCorpusChristi(dataAtual.ano) == 5) {
        display.print(String(DiaCorpusChristi(dataAtual.ano)) + " - Corpus Christi");
      }
      break;
    case 6:
      if (MesCorpusChristi(dataAtual.ano) == 6) {
        display.print(String(DiaCorpusChristi(dataAtual.ano)) + " - Corpus Christi");
      }
      break;
    case 7:
      break;
    case 8:
      break;
    case 9:
      display.print("7 - Independência do Brasil");
      break;
    case 10:
      display.print("12 - Nossa Sra Aparecida");
      break;
    case 11:
      display.print("2 - Dia de Finados\n");
      display.print("15 - Proclamação da República\n");
      display.print("20 - Dia Nacional de Zumbi e da Consciência Negra");
      break;
    case 12:
      display.print("25 - Natal");
      break;
  }
}

int DiaPascoa(int Y) {
  float A, B, C, P, Q, M, N, D, E;

  A = Y % 19;
  B = Y % 4;
  C = Y % 7;
  P = floor((float)Y / 100.0);
  Q = floor((float)(13 + 8 * P) / 25.0);
  M = (int)(15 - Q + P - (floor)(P / 4)) % 30;
  N = (int)(4 + P - (floor)(P / 4)) % 7;
  D = (int)(19 * A + M) % 30;
  E = (int)(2 * B + 4 * C + 6 * D + N) % 7;

  int days = (int)(22 + D + E);

  // A corner case,
  // when D is 29
  if ((D == 29) && (E == 6)) {
    return 19;
  }
  // Another corner case,
  // when D is 28
  else if ((D == 28) && (E == 6)) {
    return 18;
  } else {
    // If days > 31, move to April
    // April = 4th Month
    if (days > 31) {
      return days - 31;
    } else {
      return days;
    }
  }
}

int MesPascoa(int Y) {
  float A, B, C, P, Q, M, N, D, E;

  A = Y % 19;
  B = Y % 4;
  C = Y % 7;
  P = floor((float)Y / 100.0);
  Q = floor((float)(13 + 8 * P) / 25.0);
  M = (int)(15 - Q + P - (floor)(P / 4)) % 30;
  N = (int)(4 + P - (floor)(P / 4)) % 7;
  D = (int)(19 * A + M) % 30;
  E = (int)(2 * B + 4 * C + 6 * D + N) % 7;
  int days = (int)(22 + D + E);

  // A corner case,
  // when D is 29
  if ((D == 29) && (E == 6)) {
    return 4;
  }
  // Another corner case,
  // when D is 28
  else if ((D == 28) && (E == 6)) {
    return 4;
  } else {
    // If days > 31, move to April
    // April = 4th Month
    if (days > 31) {
      return 4;
    } else {
      // Otherwise, stay on March
      // March = 3rd Month
      return 3;
    }
  }
}

int DiaCarnaval(int year) {
  int day = DiaPascoa(year);
  int month = MesPascoa(year);
  int totaldays = 0;
  for (int i = 0; i < month - 1; i++) {
    totaldays += diasMeses[i];
  }
  totaldays += day;
  totaldays -= 47;
  if (totaldays > (31 + diasMeses[1])) {
    totaldays -= (31 + diasMeses[1]);
    return totaldays;
  } else {
    return totaldays - 31;
  }
}

int MesCarnaval(int year) {
  int day = DiaPascoa(year);
  int month = MesPascoa(year);
  int totaldays = 0;
  for (int i = 0; i < month - 1; i++) {
    totaldays += diasMeses[i];
  }
  totaldays += day;
  totaldays -= 47;
  if (totaldays > (31 + diasMeses[1])) {
    return 3;
  } else {
    return 2;
  }
}

int DiaCorpusChristi(int year) {
  int day = DiaPascoa(year);
  int month = MesPascoa(year);
  int totaldays = 0;
  for (int i = 0; i < month - 1; i++) {
    totaldays += diasMeses[i];
  }
  totaldays += day;
  totaldays += 60;
  if (totaldays > (123 + diasMeses[1])) {
    totaldays -= (123 + diasMeses[1]);
    return totaldays;
  } else {
    return totaldays - (92 + diasMeses[1]);
  }
}

int MesCorpusChristi(int year) {
  int day = DiaPascoa(year);
  int month = MesPascoa(year);
  int totaldays = 0;
  for (int i = 0; i < month - 1; i++) {
    totaldays += diasMeses[i];
  }
  totaldays += day;
  totaldays += 60;
  if (totaldays > (123 + diasMeses[1])) {
    return 6;
  } else {
    return 5;
  }
}