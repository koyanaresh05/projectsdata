
exports = function(payload){  

    const mongodb = context.services.get("mongodb-atlas");
  const itemsCollection = mongodb.db("ASSET_INFO").collection("New_Luggage_data");
    // Querying a mongodb service:
   //  const doc = context.services.get("mongodb-atlas").db("ASSET_INFO").collection("New_Luggage_data").findOne();
  //   const doc = context.services.get("mongodb-atlas").db("ASSET_INFO").collection("New_Luggage_data").find();
  
  //   const doc = context.services.get("mongodb-atlas").db("ASSET_INFO").collection("New_Luggage_data").find({"Asset Status":"Safe"});
  
   //  const doc = context.services.get("mongodb-atlas").db("ASSET_INFO").collection("New_Luggage_data").find({"Asset Status":"UnSafe"});  
  // var doc = context.services.get("mongodb-atlas").db("ASSET_INFO").collection("New_Luggage_data").find({"Tag_id":payload.body.text()});
   // doc = doc._id.toString();

 // var doc = context.services.get("mongodb-atlas").db("ASSET_INFO").collection("New_Luggage_data").find({"Tag_id":payload});
 const doc = itemsCollection.findOne({"Tag_id":payload.body.text()},{_id:0});
 // const doc = itemsCollection.find({"type":"unsafe luggage data"},{_id:0});
 
   // const name1 = doc.Source;
   return  doc;
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