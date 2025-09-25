# Ground Station (GS)

A **Ground Station (GS)** é um sistema embarcado desenvolvido pelo **Departamento de Sistemas Embarcados** que funciona como ponte de comunicação entre:  

- O **URD_APP** e o **WiFIre** (ignição).  
- O **URD_APP** e o **software de voo** (telemetria).  

O código foi projetado para receber e retransmitir comandos via **LoRa** e/ou **Serial**, podendo ser utilizado tanto para o **Arduino Mega** quanto para o **Esp32**.  

---

## 📡 Funcionalidades Principais

- **Recepção via LoRa:**  
  - Recebe comandos e dados, enviando-os para o URD_APP ou exibindo-os na Serial.  

- **Recepção via Serial:**  
  - Aceita comandos diretamente pelo computador/serial para executar ações.  
  - Suporta comandos de ignição, iguais aos do **WiFire**.  

Os comandos são:
- `READY` → Inicia a recepção de dados. 
- `RST` → Reinicia o microcontrolador (utilizado pelo **URD_APP**). 
- `PING!\n` → Comando de teste enviado pela base.  
- `PONG!\n` → Resposta do sistema indicando comunicação ativa.  
- `ARMED!\n` → Coloca o sistema em **estado armado** (após senha correta).  
- `DISARMED!\n` → Retorna o sistema para **estado desarmado**.  
- `IGN!\n` → Executa o comando de **ignição** (apenas se armado).   

---

## 🔊 Sinais Sonoros (Buzzer)

O buzzer da GS fornece feedback sonoro sobre o estado do sistema:  

- **Inicialização:**  
  - 3 apitos rápidos → inicialização concluída com sucesso.  
  - 1 apito longo → erro na inicialização do cartão SD.  

- **Recepção de dados:**  
  - 1 apito curto → dado recebido.  
  - +1 apito curto → dado salvo com sucesso no cartão SD.  
  - *(total de 2 apitos quando o dado é recebido e salvo).*  

---

## ⚠️ Status Atual

- O código encontra-se **estável e funcional**.  
- Comunicação via **URD_APP / Serial ↔ Microcontrolador(LoRa) ↔ WiFire** já implementada.  
- Comunicação via **URD_APP / Serial ↔ Microcontrolador(LoRa) ↔ Software de voo** em desenvolvimento.  
- Sistema de apitos e registro em SD já testado e validado.  

---

