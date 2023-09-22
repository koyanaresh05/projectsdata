exports = function(payload){
  const mongodb = context.services.get("mongodb-atlas");
  const itemsCollection = mongodb.db("ASSET_INFO").collection("OTP_Data");
  const query = { "Mob": payload.body.text()};
  
  itemsCollection.deleteOne(query);
  
      var atlas = context.services.get("mongodb-atlas");
      var coll = atlas.db("ASSET_INFO").collection("OTP_Data");
      const doc = coll.findOne({"Mob":"OTP Generator"},{_id:0});
      var num_OTP = parseInt(doc.OTP) - 1 ;
      if(num_OTP<0)
      num_OTP=0;
      
      num_OTP = num_OTP + "";// for converting into string
      
      const query2 = { "Mob":"OTP Generator"};
      const update = {
      "$set": {
        "OTP": num_OTP
      }
      };
      const options = { "upsert": false };// upsert  means update or (insert if field unavaible)
  coll.updateOne(query2, update, options);//update the incremented num_OTP
    
};