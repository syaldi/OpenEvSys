<?php global $conf; 
    include_once('tabs.php');
    include_once('docu_title.php');    
?>
<?php
    include_once('view_card_list.php');
    draw_card_list('lk');  
?>
<div class="panel">
<table class='table table-bordered table-striped table-hover'>
    <thead>
        <tr>
            <th><?php echo _t('RECORD_TYPE')?></th>
            <th><?php echo _t('RECORD_NUMBER')?></th>
            <th><?php echo _t('BY_WHOM')?></th>
            <th><?php echo _t('LINKED_WHEN')?></th>
        </tr>
    </thead>
    <tbody>
    <?php
        foreach($entities as $entity=>$entity_name){
            $link_docs = $links[$entity];
            
            if(!is_array($link_docs))continue;
            foreach($link_docs as $record ){
?>
                <tr>
                    <td><?php echo $entity_name?></td>
                    <td><a href="<?php echo get_record_url($record['record_number'],$entity)?>"><?php echo $record['record_number']?></a></td>
                    <td><?php echo $record['linked_by']?></td>
                    <td><?php echo $record['linked_date']?></td>
                </tr>
<?php
            }
        }
    ?>
    </tbody>
</table>
</div>
