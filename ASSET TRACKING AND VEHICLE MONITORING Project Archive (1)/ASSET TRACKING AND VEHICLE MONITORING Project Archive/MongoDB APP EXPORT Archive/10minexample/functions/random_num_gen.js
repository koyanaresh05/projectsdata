exports = function(payload){

        const rn = parseInt(Math.random() * 10000);
        var rn2 ="";
        if(rn<10)
        {
           rn2 = "000" + rn;
        }
        else if(rn<100)
        {
           rn2 = "00" + rn;
        }
        else if(rn<1000)
        {
           rn2 = "0" + rn;
        }
        else
        {
           rn2 = "" + rn;
        }

      return rn2;

};