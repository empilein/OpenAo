USE [atum2_db_account]
GO

INSERT INTO [dbo].[ti_ItemInfo]
           ([ItemNum]
           ,[Kind]
           ,[ItemName]
           ,[AbilityMin]
           ,[AbilityMax]
           ,[ReqRace]
           ,[ReqAttackPart]
           ,[ReqDefensePart]
           ,[ReqFuelPart]
           ,[ReqSoulPart]
           ,[ReqShieldPart]
           ,[ReqDodgePart]
           ,[ReqUnitKind]
           ,[ReqMinLevel]
           ,[ReqMaxLevel]
           ,[ReqItemKind]
           ,[Weight]
           ,[HitRate]
           ,[Defense]
           ,[FractionResistance]
           ,[NaturalFaction]
           ,[SpeedPenalty]
           ,[Range]
           ,[Position]
           ,[Scarcity]
           ,[Endurance]
           ,[AbrasionRate]
           ,[Charging]
           ,[Luck]
           ,[MinTradeQuantity]
           ,[Price]
           ,[CashPrice]
           ,[DestParameter1]
           ,[ParameterValue1]
           ,[DestParameter2]
           ,[ParameterValue2]
           ,[DestParameter3]
           ,[ParameterValue3]
           ,[DestParameter4]
           ,[ParameterValue4]
           ,[DestParameter5]
           ,[ParameterValue5]
           ,[DestParameter6]
           ,[ParameterValue6]
           ,[DestParameter7]
           ,[ParameterValue7]
           ,[DestParameter8]
           ,[ParameterValue8]
           ,[ReAttacktime]
           ,[Time]
           ,[RepeatTime]
           ,[Material]
           ,[ReqMaterial]
           ,[RangeAngle]
           ,[UpgradeNum]
           ,[LinkItem]
           ,[MultiTarget]
           ,[ExplosionRange]
           ,[ReactionRange]
           ,[ShotNum]
           ,[MultiNum]
           ,[AttackTime]
           ,[ReqSP]
           ,[SummonMonster]
           ,[NextSkill]
           ,[SkillLevel]
           ,[SkillHitRate]
           ,[SkillType]
           ,[SkillTargetType]
           ,[Caliber]
           ,[OrbitType]
           ,[ItemAttribute]
           ,[BoosterAngle]
           ,[CameraPattern]
           ,[SourceIndex]
           ,[Description]
           ,[EnchantCheckDestParam]
           ,[InvokingDestParamID]
           ,[InvokingDestParamIDByUse]
           ,[Enabled])
     SELECT 10010000 + a.CodeNum as ItemNum
      ,[Kind]
      ,case when CHARINDEX('of ', a.Name) = 1 then '[SUFFIX] ' + substring(a.Name, 4, 100) else '[PREFIX] ' + a.Name end as Name
      ,[AbilityMin]
           ,[AbilityMax]
           ,[ReqRace]
           ,ti_item.[ReqAttackPart]
           ,ti_item.[ReqDefensePart]
           ,ti_item.[ReqFuelPart]
           ,ti_item.[ReqSoulPart]
           ,ti_item.[ReqShieldPart]
           ,ti_item.[ReqDodgePart]
           ,ti_item.[ReqUnitKind]
           ,ti_item.[ReqMinLevel]
           ,ti_item.[ReqMaxLevel]
           ,40
           ,[Weight]
           ,[HitRate]
           ,[Defense]
           ,[FractionResistance]
           ,[NaturalFaction]
           ,[SpeedPenalty]
           ,[Range]
           ,[Position]
           ,[Scarcity]
           ,[Endurance]
           ,[AbrasionRate]
           ,[Charging]
           ,[Luck]
           ,[MinTradeQuantity]
           ,[Price]
           ,[CashPrice]
      ,249
      ,a.CodeNum
      ,250
      ,b.CodeNum
      ,ti_item.[DestParameter3]
      ,ti_item.[ParameterValue3]
      ,ti_item.[DestParameter4]
      ,ti_item.[ParameterValue4]
      ,ti_item.[DestParameter5]
      ,ti_item.[ParameterValue5]
      ,ti_item.[DestParameter6]
      ,ti_item.[ParameterValue6]
      ,ti_item.[DestParameter7]
      ,ti_item.[ParameterValue7]
      ,ti_item.[DestParameter8]
      ,ti_item.[ParameterValue8]
      ,[ReAttacktime]
      ,[Time]
      ,[RepeatTime]
      ,[Material]
      ,[ReqMaterial]
      ,[RangeAngle]
      ,[UpgradeNum]
      ,[LinkItem]
      ,[MultiTarget]
      ,[ExplosionRange]
      ,[ReactionRange]
      ,[ShotNum]
      ,[MultiNum]
      ,[AttackTime]
      ,[ReqSP]
      ,[SummonMonster]
      ,[NextSkill]
      ,[SkillLevel]
      ,[SkillHitRate]
      ,[SkillType]
      ,[SkillTargetType]
      ,[Caliber]
      ,[OrbitType]
      ,[ItemAttribute]
      ,[BoosterAngle]
      ,[CameraPattern]
      ,[SourceIndex]
      ,'This card will give the ' + case when CHARINDEX('of ', a.Name) = 1 then substring(a.Name, 4, 100) + ' suffix' else a.Name + ' prefix' end  + ' to your weapon.' as Description
      ,[EnchantCheckDestParam]
      ,[InvokingDestParamID]
      ,[InvokingDestParamIDByUse]
	  ,1
  FROM [atum2_db_account].[dbo].[ti_ItemInfo] ti_item, ti_rareiteminfo a
left join ti_rareiteminfo b
on a.Name like b.Name and a.CodeNum <> b.CodeNum
where ti_item.itemnum = 7034490 and a.ReqItemKind = 44 and b.ReqItemKind = 48
GO


