window.requestAnimFrame = (function(callback) {
        return window.requestAnimationFrame || 
        window.webkitRequestAnimationFrame || 
        window.mozRequestAnimationFrame || 
        window.oRequestAnimationFrame || 
        window.msRequestAnimationFrame ||
        function(callback) {
          window.setTimeout(callback, 1000 / 30); // 30 frames per second
   };
 })();
 var pollOneH = 0;
 var poll1;
 var text;
 var potValue;
 var prevPotValue;
 var toggleVal = 10;
function changeValue(val)
{
pollOneH=val*2.25;
}


function animation(poll1,text)
 {
   var canvas = document.getElementById("myCanvas");
          var content = canvas.getContext("2d");
   
   // clear canvas
   content.clearRect(0, 0, 140, 240);
   
   
   
   // make the wobbely values stop 
   if(pollOneH*2 > prevPotValue+2 || pollOneH*2 < prevPotValue-2){
  prevPotValue = potValue;
  potValue = pollOneH*2;
   }
   if(Math.round(potValue/4.5)>=60){ //green
     content.fillStyle = '#4CAF50';
   }
   if(Math.round(potValue/4.5)<60&&Math.round(potValue/4.5)>20){//orange
     content.fillStyle = '#FF9800';
   }
   if(Math.round(potValue/4.5)<=20){//red
     content.fillStyle = '#D32F2F';
   }
   content.textAlign = 'center';
   content.font = '10pt Calibri';
   
  // content.fillStyle = 'orange';
   
   document.getElementById("BS").setAttribute("data-badge",Math.round(potValue/4.5))
   content.fillText('Power level : ' + Math.round(potValue/4.5)+'%', text.x, text.y);
   // rect. graph 
   content.fillRect(poll1.x,(poll1.y-poll1.h),poll1.w,poll1.h);
   content.fill();
   
   

   // request new frame
   requestAnimFrame(function() {
   if(poll1.h < pollOneH){
  poll1.h += (pollOneH - poll1.h)*.15;
   }
   else if(poll1.h > pollOneH){
  poll1.h -= (poll1.h - pollOneH)*.15;
   }
   text.y = (poll1.y - poll1.h) -5;
   animation(poll1,text);
  });
 }
function initPoll()
 {
    poll1 = {
  x: 11,
  y: 245,
  w: 120,
  h: 0
    }
    text = {
  x:poll1.w/2+10,
  y:100
    } 
    
    potValue = pollOneH*2;
    prevPotValue = potValue;
    animation(poll1,text);
 }
window.onload = function() {
initPoll();
};