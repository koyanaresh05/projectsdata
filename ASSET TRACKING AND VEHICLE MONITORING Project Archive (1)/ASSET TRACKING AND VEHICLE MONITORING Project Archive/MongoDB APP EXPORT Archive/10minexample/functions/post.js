exports = function(payload){
  
  /*Two tasks are being performed
    Updating the lock status
    Inserting the document For Tag_id
  */
   /*For Updating lock status*/     
    const mongodb = context.services.get("mongodb-atlas");
    const itemsCollection = mongodb.db("ASSET_INFO").collection("Lock_Status");
    const doc = EJSON.parse(payload.body.text());
    const new_Tag_id = doc.Tag_id;
    
    const query = { "Tag_id": new_Tag_id};
    const update = {
      "$set": {
        "Lock_Status": "1"
      }
    };
    const options = { "upsert": false };// upsert  means update or (insert if field unavaible)
    
    itemsCollection.updateOne(query, update, options)


  /*For Inserting document for new location SMS */
    var atlas1 = context.services.get("mongodb-atlas");
    
    var coll1 = atlas1.db("ASSET_INFO").collection("OTP_Data");
   
    var doc2 = coll1.findOne({"Mob":"OTP Generator"},{_id:0});
    var num_OTP = parseInt(doc2.OTP) + 1 ;
    num_OTP = num_OTP + "";// for converting into string
    
    const query2 = { "Mob":"OTP Generator"};
    const update2 = {
      "$set": {
        "OTP": num_OTP
      }
    };
    const options2 = { "upsert": false };// upsert  means update or (insert if field unavaible)
    coll1.updateOne(query2, update2, options2);//update the incremented num_OTP
   const doc3 = EJSON.parse(payload.body.text());
    var SMS_str ="Your TAG ID for current travel is : " + doc3.Tag_id;
    
   doc2["Mob"]=doc3.Contact;
   doc2["OTP"]=SMS_str;
      doc2['valid'] = "true"; // This is a dummy key which will help to find latest earliest doc with mob and otp in get_OTP fun
       doc2['ts'] = new Date();
        

      coll1.insertOne(doc2);//Insert new doc containing the new OTP and mob no 
      
    


  /*For Inserting document for Tag_id*/
    var atlas = context.services.get("mongodb-atlas");
    var coll = atlas.db("ASSET_INFO").collection("New_Luggage_data");

    
    try {
      if (payload.body) 
      {
        //const ts = {ts:new Date()};
        //const ts_ej = EJSON.stringify(ts);
        body = EJSON.parse(payload.body.text());
        body['ts'] = new Date();
       // body['Lock_Status'] = "1";//Not needed as seperate collection is made for lock status which was necessary to be available even if tag entry is deleted
      }
      coll.insertOne(body);
      console.log(body);

    } catch (e) {
      console.log("Error inserting doc: " + e);
      return e.message();
    }
};