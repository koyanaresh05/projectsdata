exports = function(payload) {
  const mongodb = context.services.get("mongodb-atlas");
  const itemsCollection = mongodb.db("ASSET_INFO").collection("New_Luggage_data");
  const doc = itemsCollection.findOne({"module":"GPS"}); 
  const last_loc_str = doc.last_loc;
  const query = { "Tag_id": payload.body.text()};
//const query = { "Tag_id": "bid_qr_001"};
const update = {
  "$set": {
    "Asset Status": "Unsafe",
    "last_loc":last_loc_str 
  }
};
const options = { "upsert": false };// upsert  means update or (insert if field unavaible)

itemsCollection.updateOne(query, update, options);


  /*For Inserting document for new location SMS */
    var atlas = context.services.get("mongodb-atlas");
    
    var coll = atlas.db("ASSET_INFO").collection("OTP_Data");
   
    var doc2 = coll.findOne({"Mob":"OTP Generator"},{_id:0});
    var num_OTP = parseInt(doc2.OTP) + 1 ;
    num_OTP = num_OTP + "";// for converting into string
    
    const query2 = { "Mob":"OTP Generator"};
    const update2 = {
      "$set": {
        "OTP": num_OTP
      }
    };
    const options2 = { "upsert": false };// upsert  means update or (insert if field unavaible)
    coll.updateOne(query2, update2, options2);//update the incremented num_OTP
   
    var SMS_str ="ALERT: Your asset is unsafe!!!, the last connected location for current travel was : " + last_loc_str;
    const doc3 = itemsCollection.findOne({"Tag_id": payload.body.text()}); 
//  const doc3 = itemsCollection.findOne({"Tag_id": "bid_qr_001"});   
   doc2["Mob"]=doc3.Contact;
   doc2["OTP"]=SMS_str;
      doc2['valid'] = "true"; // This is a dummy key which will help to find latest earliest doc with mob and otp in get_OTP fun
       doc2['ts'] = new Date();
        

      coll.insertOne(doc2);//Insert new doc containing the new OTP and mob no 
      
    
    
return doc.last_loc;

}







/*
 const doc = itemsCollection.find({"type":"unsafe luggage data"},{_id:0});
var x= doc.index;
var y = parseInt(x) + 1;
const z = doc.name + ',' + payload;
*/
/*
var obj2 = {};
var key = z;
obj2[key] = doc.name + ',' + payload;
//{someKey: 'someValue'}


const query2 = { "type": "unsafe luggage data"};
const update2 = {
  "$set": {
      "name": z,
      "index": y
   
  }
  
 
};
const options2 = { "upsert": true };// upsert  means update or (insert if field unavaible)

itemsCollection.updateOne(query2, update2, options2);


   return  z;
   
 */ 


/*

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
 const doc = itemsCollection.findOne({"type":"unsafe luggage data"});
   // const name1 = doc.Source;

//var x= doc.index;
// var y = parseInt(x) + 3;
   return  doc;
};

*/
