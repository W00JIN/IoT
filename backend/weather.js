const request = require('request')
const cheerio = require('cheerio')
var ws_artik = require('ws');
var ws_artik_server = new ws_artik.Server({port:80});

ws_artik_server.on('connection', function(ws) {
  console.log("artik is connected")
  let url = "https://weather.naver.com/rgn/cityWetrWarea.nhn?cityRgnCd=CT001000"
  request(url, function(error, response, body){
    if (error) {
      console.log(error)
      return;
    }
    const $ = cheerio.load(body)
    let weather=[], temperature=[], precipitation=[]
    let tr = $('.tbl_weather > tbody > tr').each(function (index, elem) {
      let city = $(this).find('th > a').text()
      if(city == "서울"){
        let td = $(this).find('td').each(function (index, elem) {
          if(index == 0){
            weather.push($(this).find('ul > li').first().text())
            let list = $(this).find('ul > li > span > strong').each(function (index, elem) {
              if(index == 0) temperature.push($(this).text())
              else if(index == 1) precipitation.push($(this).text())
            })
          }
          if(index == 1){
            weather.push($(this).find('ul > li').first().text())
            let list = $(this).find('ul > li > span > strong').each(function (index, elem) {
              if(index == 0) temperature.push($(this).text())
              else if(index == 1) precipitation.push($(this).text())
            })
          }
        })

      }
    })
    //console.log("{\"morning\": {\"weather\": \""+weather[0]+"\", \"temperature\": \""+temperature[0]+"\", \"precipitation\": \""+precipitation[0]+"\"}, \"evening\": {\"weather\": \""+weather[1]+"\", \"temperature\": \""+temperature[1]+"\", \"precipitation\": \""+precipitation[1]+"\"} }")
    //ws.send("{\"morning\": {\"weather\": \""+weather[0]+"\", \"temperature\": \""+temperature[0]+"\", \"precipitation\": \""+precipitation[0]+"\"}, \"evening\": {\"weather\": \""+weather[1]+"\", \"temperature\": \""+temperature[1]+"\", \"precipitation\": \""+precipitation[1]+"\"} }")

  console.log(weather[0]+"/"+temperature[0]+"/"+precipitation[0])
  ws.send(weather[0]+"/"+temperature[0]+"/"+precipitation[0])
  })
  ws.on('message', function message(data) {
    console.log("data : " + data)
  });

  ws.on('close', function message(data) {
    console.log('artik is disconnected');
  })
});


