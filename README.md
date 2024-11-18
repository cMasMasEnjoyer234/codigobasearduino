**FUNCIONAMIENTO GENERAL

Consiste de 3 modos: Modo clock, Modo PomodoroConfig, Modo Pomodoro (existiria un 4 pero es solo cuando se prende por primera vez para configurar la hora).
MODO CLOCK: 
El modo clock consiste en un algoritmo el cual cuenta el tiempo de los minutos , con la condicion del tiempo que el esp estuvo prendido contra el actual, contando cada 60.000mili segundos (esto seria un minuto), este segun donde este, sea el modo que sea, siempre se mantendra contando sin importar el modo.

MODO POMODORO_CONFIG
Este modo consiste en la configuracion de el pomodoro, tiene 3 fases:
	-fase 1: ¿Cuantos ciclos se repetiran? 
    Puedes con el potenciometro o con el cellular decider del 1 al 8, cuantas veces se repetira             
    el ciclo.
  -fase 2: ¿Cuantos minutos descansaras?
 		Aca con los mismos inputs puedes decider desde 1 a 15 minutos de Descanso.
	-fase 3: ¿Cuantos minutos vas a trabajar?
    Por ultimo, esta fase determina cuantos minutos vas a trabajar por ciclo con un intervalo entre  10 a 59 minutos.
    (si no estas satisfecho o colocaste un valor por error, no te preocupes, el boton que sirve para configurar este modo esta diseñado para empesar el pomodoro solo si estas en la fase 3 y mantienes el boton derecho unos segundos, por lo que puedes navegar entre fases libremente).

MODO POMODORO:
Una vez de que este satisfecho con la configuracion del pomodoro, mantienes apretado el boton derecho y automaticamente se activara este modo, en cuanto estes en el Descanso, sonara una alarma indicandote que descanses. Si no quieres usarlo y po pusiste por error o solo quieres Volver al clock, presionas el boton izquierdo y automaticamente te llevara a la hora, las configs no se guardan por lo que tendras que Volver a configurar el pomodoro.
