exports = function(payload){
  
  /*For Inserting document for Bus*/
    var atlas = context.services.get("mongodb-atlas");
    var coll = atlas.db("BUS_INFO").collection("Bus_exp2_Data");
  
    const doc = EJSON.parse(payload.body.text());
    doc['ts'] = new Date();
    coll.insertOne(doc);
    
   

};