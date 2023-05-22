# Objetos-inteligetes-conectados
Repositório para a entrega final do projeto da matéria de objetos inteligentes conectados.
The Hospital Lights
O projeto visa possibilitar que pessoas em hospitais, consigam acender ou apagar a luz de seus quartos, apenas utilizando um botão na tela de seu celular, através de um Sensor LDR, que faz a captura da sensibilidade da LUZ e o envio da informação para o atuador (Relé). Por sua         vez, o Rele faz o envio para o Arduíno, que através do protocolo MQTT, envia as informações para a rede. O subscriber, que seria o celular com o MQTT, vai definir se o botão será utilizado para ascender ou apagar a lâmpada, de acordo com a sensibilidade da luz, detectada no ínicio, pelo sensor de luminosidade.
