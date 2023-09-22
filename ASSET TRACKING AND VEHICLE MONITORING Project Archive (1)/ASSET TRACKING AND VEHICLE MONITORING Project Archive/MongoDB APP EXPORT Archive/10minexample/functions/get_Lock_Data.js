
exports = function(payload){  

    const mongodb = context.services.get("mongodb-atlas");
  const itemsCollection = mongodb.db("ASSET_INFO").collection("Lock_Status");

 const doc = itemsCollection.findOne({"Tag_id":payload.body.text()},{_id:0});
//  const doc = itemsCollection.findOne({"Tag_id":payload},{_id:0});

   // const name1 = doc.Source;
 //  if(doc.Lock_Status == "1")
//   return "1";
//   else
//   return "0";

  return doc.Lock_Status;
};




/*
exports = function(payload){
    var atlas = context.services.get("mongodb-atlas");
    var coll = atlas.db("ASSET_INFO").collection("New_Luggage_data");
    try {
      if (payload.body) 
      {
        //const ts = {ts:new Date()};
        //const ts_ej = EJSON.stringify(ts);
        body = EJSON.parse(payload.body.text());
        body['ts'] = new Date();
      }
      coll.insertOne(body);
      console.log(body);

    } catch (e) {
      console.log("Error inserting doc: " + e);
      return e.message();
    }
};
*/