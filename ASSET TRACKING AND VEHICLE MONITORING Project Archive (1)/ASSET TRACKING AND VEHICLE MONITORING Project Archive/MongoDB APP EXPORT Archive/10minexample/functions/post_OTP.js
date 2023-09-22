exports = function(payload){
  
  /*For Inserting document for new OTP*/
    var atlas = context.services.get("mongodb-atlas");
    
    var coll = atlas.db("ASSET_INFO").collection("OTP_Data");
   
    const doc = coll.findOne({"Mob":"OTP Generator"},{_id:0});
    var num_OTP = parseInt(doc.OTP) + 1 ;
    num_OTP = num_OTP + "";// for converting into string
    
    const query = { "Mob":"OTP Generator"};
    const update = {
      "$set": {
        "OTP": num_OTP
      }
    };
    const options = { "upsert": false };// upsert  means update or (insert if field unavaible)
    
    
    try {
      if (payload.body) 
      {
        coll.updateOne(query, update, options);//update the incremented num_OTP
        body = EJSON.parse(payload.body.text());
      // body = EJSON.parse(payload);
       
         const rn = parseInt(Math.random() * 10000);
        var rn2 ="";
        if(rn<10)
        {
           rn2 = "Your One Time Password for current Query is: "+"000" + rn;
        }
        else if(rn<100)
        {
           rn2 = "Your One Time Password for current Query is: "+"00" + rn;
        }
        else if(rn<1000)
        {
           rn2 = "Your One Time Password for current Query is: "+"0" + rn;
        }
        else
        {
           rn2 = "Your One Time Password for current Query is: "+"" + rn;
        }
       
        body['OTP'] = rn2;
        body['valid'] = "true"; // This is a dummy key which will help to find latest earliest doc with mob and otp in get_OTP fun
        body['ts'] = new Date();
        
      }
      coll.insertOne(body);//Insert new doc containing the new OTP and mob no 
      console.log(body);
      return rn2;

    } catch (e) {
      console.log("Error inserting doc: " + e);
      return e.message();
    }
    
};