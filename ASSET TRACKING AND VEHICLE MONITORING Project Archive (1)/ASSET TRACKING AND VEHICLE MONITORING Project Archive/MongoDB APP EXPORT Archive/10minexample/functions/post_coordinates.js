exports = function(payload){
  
  const mongodb = context.services.get("mongodb-atlas");
  const itemsCollection = mongodb.db("BUS_INFO").collection("BUS_OBD_DATA");
  const itemsCollection2 = mongodb.db("BUS_INFO").collection("BUS_LOCATION_DATA");
  
 const count =  itemsCollection.count({"ts":null});
const doc = itemsCollection2.findOne({"locid":parseInt(payload.body.text())},{_id:0});
//const doc = itemsCollection2.findOne({"locid":  parseInt(payload)},{_id:0});

 

const query = { "ts": null};
    const update = {
      "$set": {
        
        "lattitude": doc.Y,
        "longitude": doc.X,
        "ts": new Date()
      }
    };
    const options = { "upsert": true };// upsert  means update or (insert if field unavaible)
    
    itemsCollection.updateOne(query, update, options)


return count;




  /*For Inserting document for Tag_id*/
  /*  var atlas = context.services.get("mongodb-atlas");
    var coll = atlas.db("BUS_INFO").collection("Bus_location_data");
        body = EJSON.parse(payload.body.text());
      
        body['ts'] = new Date();
     
      coll.insertOne(body);
      console.log(body);
*/

};